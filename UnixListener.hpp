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

#ifndef UNIXLISTENER_HPP
#define UNIXLISTENER_HPP

#include "Log.hpp"
#include "SingleThread.hpp"
#include "IPCConfig.hpp"
#include "IPCClient.hpp"
#include "GPSReader.hpp"
#include "MethodWrapper.hpp"
#include <list>
#include <mutex>

namespace ship_position
{

class UnixListener : public SingleThread
{
public:
    UnixListener(const IPCConfig &config, GPSReader &gpsReader);
    virtual ~UnixListener();

    virtual void run();
    virtual void stop();

    std::vector<int> getRunningClients();
protected:
    bool setup();
    void teardown();
    void onClientStopped(int clientId);
    void cleanStoppedClients();

    Log *_log;
    const IPCConfig &_config;
    int _fd;
    int _nextClientId;
    std::list<IPCClient *> _clients;
    std::mutex _stoppedClientsMutex;
    std::vector<int> _stoppedClients;
    GPSReader &_gpsReader;
};

}

#endif // UNIXLISTENER_HPP