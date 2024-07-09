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

#include "UnixListener.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>

namespace ship_position
{

UnixListener::UnixListener(const IPCConfig &config, GPSReader &gpsReader)
: _config(config),
  _fd(-1),
  _gpsReader(gpsReader),
  _nextClientId(1)
{
    _log = Log::getInstance();
}

UnixListener::~UnixListener()
{
    Log::release();
}

void UnixListener::run()
{
    _log->write(LogLevel::NOTICE, "UnixListener::run()\n");

    if (setup() != true)
    {
        teardown();
        return;
    }

    while (true)
    {
        if (need_to_stop() == true)
        {
            break;
        }

        cleanStoppedClients();

        int clientsock = accept(_fd, nullptr, nullptr);
        if (clientsock == -1)
        {
            _log->write(LogLevel::NOTICE, "UnixListener failed to accept connection, error code %d\n", errno);
            continue;
        }

        IPCClient *client = new IPCClient(_nextClientId++, clientsock, _config, _gpsReader,
            methodWrapper<UnixListener, void, int>(this, &UnixListener::onClientStopped));
        _clients.push_back(client);
        client->start();
    }
}

void UnixListener::stop()
{
    teardown();
    SingleThread::stop();
}

std::vector<int> UnixListener::getRunningClients()
{
    std::vector<int> result;
    for (auto client : _clients)
    {
        result.push_back(client->getId());
    }
    return result;
}

bool UnixListener::setup()
{
    _fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (_fd == -1)
    {
        _log->write(LogLevel::ERROR, "UnixListener failed to open socket, error code %d\n", errno);
        return false;
    }
    sockaddr_un addr;
    std::memset(&addr, 0, sizeof(sockaddr_un));
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, _config.socketPath.c_str(), sizeof(addr.sun_path) - 1);

    _log->write(LogLevel::DEBUG, "UnixListener opening Unix socket %s\n", addr.sun_path);

    if (bind(_fd, reinterpret_cast<const sockaddr *>(&addr), sizeof(sockaddr_un)) == -1)
    {
        _log->write(LogLevel::ERROR, "UnixListener failed to bind socket, error code %d\n", errno);
        return false;
    }

    if (listen(_fd, 32) == -1)
    {
        _log->write(LogLevel::ERROR, "UnixListener failed to listen socket, error code %d\n", errno);
        return false;
    }

    return true;
}

void UnixListener::teardown()
{
    _log->write(LogLevel::NOTICE, "UnixListener::stop()\n");

    for (IPCClient *client : _clients)
    {
        client->stop();
    }

    cleanStoppedClients();

    if (_fd != -1)
    {
        shutdown(_fd, SHUT_RDWR);
        _fd = -1;
        unlink(_config.socketPath.c_str());
    }
}

void UnixListener::onClientStopped(int clientId)
{
    _log->write(LogLevel::NOTICE, "UnixListener::onClientStopped(%d)\n", clientId);

    std::unique_lock<std::mutex> lock(_stoppedClientsMutex);
    _stoppedClients.push_back(clientId);
}

void UnixListener::cleanStoppedClients()
{
    _log->write(LogLevel::NOTICE, "UnixListener::cleanStoppedClients()\n");

    std::unique_lock<std::mutex> lock(_stoppedClientsMutex);
    for (int clientId : _stoppedClients)
    {
        for (auto it = _clients.begin(); it != _clients.end(); ++it)
        {
            if ((*it)->getId() == clientId)
            {
                delete *it;
                _clients.erase(it);
                break;
            }
        }
    }
    _stoppedClients.clear();
}

}