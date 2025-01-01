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

#include "IPCClient.hpp"
#include "IPCMessages.hpp"
#include <unistd.h>

using json = nlohmann::json;

namespace ship_position
{

IPCClient::IPCClient(int id, int fd, const IPCConfig &config, GPSReader &gpsReader,
    MagnetometerReader &magnetometerReader, std::function<void(int)> stopCb)
: _id(id),
_fd(fd),
_config(config),
_gpsReader(gpsReader),
_magnetometerReader(magnetometerReader),
_stopCb(stopCb)
{
    _buf = static_cast<char *>(new char[_config.bufSize]);
    _log = Log::getInstance();
}

IPCClient::~IPCClient()
{
    Log::release();
    delete _buf;
}

void IPCClient::run()
{
    if (_fd == -1)
    {
        return;
    }

    _log->write(LogLevel::NOTICE, "IPCClient %d started\n", _id);

    while (true)
    {
        if (need_to_stop() == true)
        {
            break;
        }

        int len = read(_fd, reinterpret_cast<void *>(_buf), _config.bufSize - 1);
        if (len == -1)
        {
            _log->write(LogLevel::ERROR, "IPCClient %d failed to read data from socket, error code %d\n", _id, errno);
            break;
        }
        if (len == 0)
        {
            _log->write(LogLevel::NOTICE, "IPCClient %d failed to read data from socket, socket closed\n", _id);
            break;
        }
        _buf[len] = '\0';
        std::string resp = handleRequest(std::string(_buf));

        if (write(_fd, reinterpret_cast<const void *>(resp.c_str()), resp.length()) == -1)
        {
            _log->write(LogLevel::ERROR, "IPCClient %d failed to write data into socket, error code %d\n", _id, errno);
            break;
        }
    }

    close(_fd);
    _stopCb(_id);

    _log->write(LogLevel::NOTICE, "IPCClient %d stopped\n", _id);
}

void IPCClient::stop()
{
    SingleThread::stop();
}

std::string IPCClient::handleRequest(const std::string &rq)
{
    _log->write(LogLevel::DEBUG, "IPCClient %d handling request %s\n", _id, rq.c_str());
    try
    {
        json json_rq = json::parse(rq);
        IPCRequest ipcRq = json_rq.get<IPCRequest>();

        if (ipcRq.cmd == ipcRq.cmdGetGPS)
        {
            GPSInfo gpsInfo;
            _gpsReader.getGPSInfo(gpsInfo);
            GPSInfoResponse resp(gpsInfo);
            json json_resp = resp;
            std::string respStr = json_resp.dump();
            _log->write(LogLevel::DEBUG, "IPCClient %d sending response %s\n", _id, respStr.c_str());
            return respStr;
        }
        else if (ipcRq.cmd == ipcRq.cmdGetMagnetometer)
        {
            MagnetometerData magnetometerData;
            _magnetometerReader.GetMagnetometerData(magnetometerData);
            MagnetometerInfoResponse resp(magnetometerData);
            json json_resp = resp;
            std::string respStr = json_resp.dump();
            _log->write(LogLevel::DEBUG, "IPCClient %d sending response %s\n", _id, respStr.c_str());
            return respStr;
        }
        else
        {
            ErrorResponse resp;
            resp.errorMessage = "invalid command " + ipcRq.cmd;
            json json_resp = resp;
            return json_resp.dump();
        }
    }
    catch (const std::exception &e)
    {
        _log->write(LogLevel::ERROR, "IPCClient %d failed to parse request %s\n", _id, rq.c_str());
        ErrorResponse resp;
        resp.errorMessage = "failed to parse request " + rq;
        json json_resp = resp;
        return json_resp.dump();
    }
}

}