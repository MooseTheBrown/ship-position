/*
 * Copyright (C) 2024 - 2025 Mikhail Sapozhnikov
 *
 * This file is part of ship-position.
 *
 * ship-position is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ship-position is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ship-position.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "ShipPosition.hpp"
#include <signal.h>
#include <cstring>
#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

extern void signal_handler(int sig);

namespace ship_position
{

ShipPosition::ShipPosition() :
_syslog(nullptr),
_consoleLog(nullptr),
_config(nullptr),
_stopRequested(false),
_bn880gpsReader(nullptr),
_qmc5883lReader(nullptr),
_unixListener(nullptr)
{
    _log = Log::getInstance();
}

ShipPosition::~ShipPosition()
{
    Log::release();

    delete _syslog;
    delete _consoleLog;
    delete _config;
    delete _bn880gpsReader;
    delete _qmc5883lReader;
    delete _unixListener;
}

int ShipPosition::run(int argc, char **argv)
{
    _log->write(LogLevel::NOTICE, "Ship position starting\n");

    int ret = init(argc, argv);
    if (ret != RETVAL_OK)
    {
        if (ret == RETVAL_HELP)
        {
            return RETVAL_OK;
        }
        else
        {
            _log->write(LogLevel::ERROR, "ShipPosition::init() returned %d\n", ret);
            return ret;
        }
    }

    _bn880gpsReader->start();
    _qmc5883lReader->start();
    _unixListener->start();

    while (true)
    {
        if (_stopRequested)
        {
            _log->write(LogLevel::NOTICE, "Ship position stopping\n");
            _unixListener->stop();
            _bn880gpsReader->stop();
            break;
        }

        std::vector<int> clients = _unixListener->getRunningClients();
        _log->write(LogLevel::NOTICE, "Number of IPC clients: %d\n", clients.size());

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return RETVAL_OK;
}

void ShipPosition::stop()
{
    _stopRequested = true;
}

int ShipPosition::init(int argc, char **argv)
{
    int ret = handleCmdLine(argc, argv);
    if (ret != RETVAL_OK)
    {
        return ret;
    }

    _config = new Config(_configPath);
    if (!_config->isOk())
    {
        return RETVAL_INVALID_CONFIG;
    }

    // setup logging
    if (_config->isSyslogEnabled())
    {
        _syslog = new SysLog();
        _log->add_backend(_syslog);
    }
    if (_config->isConsoleLogEnabled())
    {
        _consoleLog = new ConsoleLog();
        _log->add_backend(_consoleLog);
    }
    _log->set_level(_config->getLogLevel());

    _config->getBN880GPSConfig(_bn880gpsConfig);
    _config->getQMC5883LConfig(_qmc5883lConfig);
    _config->getIPCConfig(_ipcConfig);

    _bn880gpsReader = new BN880GPSReader(_bn880gpsConfig);

    _qmc5883lReader = new QMC5883LReader(_qmc5883lConfig);

    _unixListener = new UnixListener(_ipcConfig, *_bn880gpsReader, *_qmc5883lReader);

    setupSignals();

    return RETVAL_OK;
}

int ShipPosition::handleCmdLine(int argc, char **argv)
{
    try
    {
        po::variables_map opts;
        po::options_description opt_descr("Available options:");

        opt_descr.add_options()
            ("help", "print help message")
            ("config", po::value<std::string>(), "config file path");

        po::store(po::parse_command_line(argc, argv, opt_descr), opts);
        po::notify(opts);

        if (opts.count("help"))
        {
            std::cout << "Usage: " << argv[0] << " [options]\n";
            std::cout << opt_descr << "\n";
            return RETVAL_HELP;
        }
        if (opts.count("config"))
        {
            _configPath = opts["config"].as<std::string>();
        }
        else
        {
            _configPath = DEFAULT_CONFIG_PATH;
        }

        return RETVAL_OK;
    }
    catch(const std::exception& e)
    {
        _log->write(LogLevel::ERROR, "Error processing command line: %s", e.what());
        return RETVAL_INVALID_CMDLINE;
    }
    
}

void ShipPosition::setupSignals()
{
    struct sigaction act;
    sigset_t sigset;

    // set of signals to be blocked during signal handling
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGINT);
    sigaddset(&sigset, SIGQUIT);
    sigaddset(&sigset, SIGTERM);

    act.sa_handler = signal_handler;
    act.sa_mask = sigset;
    act.sa_flags = 0;

    sigaction(SIGINT, &act, nullptr);
    sigaction(SIGQUIT, &act, nullptr);
    sigaction(SIGTERM, &act, nullptr);

    // ignore SIGPIPE
    struct sigaction ignore_act;
    std::memset(static_cast<void *>(&ignore_act), 0, sizeof(struct sigaction));
    ignore_act.sa_handler = SIG_IGN;
    ignore_act.sa_flags = 0;

    sigaction(SIGPIPE, &ignore_act, nullptr);
}

}