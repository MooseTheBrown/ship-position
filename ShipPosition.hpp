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

#ifndef SHIPPOSITION_HPP
#define SHIPPOSITION_HPP

#include "Log.hpp"
#include "Config.hpp"
#include "BN880GPSReader.hpp"
#include "UnixListener.hpp"
#include "QMC5883LReader.hpp"
#include <string>

namespace ship_position
{

class ShipPosition
{
public:
    ShipPosition();
    ~ShipPosition();

    int run(int argc, char **argv);
    void stop();

    const int RETVAL_OK = 0;
    const int RETVAL_INVALID_CONFIG = 1;
    const int RETVAL_INVALID_CMDLINE = 2;
    const int RETVAL_HELP = 3;
    const std::string DEFAULT_CONFIG_PATH = "/etc/ship-position.conf";

protected:
    int init(int argc, char **argv);
    int handleCmdLine(int argc, char **argv);
    void setupSignals();

    Log *_log;
    SysLog *_syslog;
    ConsoleLog *_consoleLog;
    Config *_config;
    std::string _configPath;
    bool _stopRequested;
    BN880GPSConfig _bn880gpsConfig;
    BN880GPSReader *_bn880gpsReader;
    QMC5883LConfig _qmc5883lConfig;
    QMC5883LReader *_qmc5883lReader;
    IPCConfig _ipcConfig;
    UnixListener *_unixListener;
};

}

#endif // SHIPPOSITION_HPP