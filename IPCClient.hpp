/*
 * Copyright (C) 2024 Mikhail Sapozhnikov
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

#ifndef IPCCLIENT_HPP
#define IPCCLIENT_HPP

#include "Log.hpp"
#include "SingleThread.hpp"
#include "IPCConfig.hpp"
#include "GPSReader.hpp"
#include <functional>

namespace ship_position
{

class IPCClient : public SingleThread
{
public:
    IPCClient(int id, int fd, const IPCConfig &config, GPSReader &gpsReader, std::function<void(int)> stopCb);
    IPCClient(const IPCClient &other) = delete;
    virtual ~IPCClient();

    virtual void run();
    virtual void stop();
    int getId() { return _id; }
protected:
    std::string handleRequest(const std::string &rq);

    int _id;
    int _fd;
    const IPCConfig &_config;
    GPSReader &_gpsReader;
    char *_buf;
    Log *_log;
    std::function<void(int)> _stopCb;
};

}

#endif // IPCCLIENT_HPP