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

#include "UnixListener.hpp"
#include "IPCMessages.hpp"
#include <gtest/gtest.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>

namespace sp = ship_position;

using json = nlohmann::json;

class TestGPSReader : public sp::GPSReader
{
public:
    virtual void getGPSInfo(sp::GPSInfo &gpsInfo)
    {
        gpsInfo.numSatellites = 10;
        gpsInfo.latitude = 56.34737;
        gpsInfo.longitude = 43.98704;
        gpsInfo.speedKnots = 1.0;
        gpsInfo.speedKm = 1.8;
    }
};

class TestMagnetometerReader : public sp::MagnetometerReader
{
public:
    virtual void GetMagnetometerData(sp::MagnetometerData &magnetometerData)
    {
        magnetometerData.x = 777;
        magnetometerData.y = 98639;
        magnetometerData.z = -84;
    }
};

class UnixListenerTest : public ::testing::Test
{
public:
    UnixListenerTest();
    virtual ~UnixListenerTest();
    virtual void SetUp();
    virtual void TearDown();
protected:
    int connectClient();
    const char *_socketPath = "/tmp/ship-position-test.socket";

    sp::IPCConfig _ipcConfig;
    sp::UnixListener *_unixListener;
    TestGPSReader _gpsReader;
    TestMagnetometerReader _magnetometerReader;
    sp::Log *_log;
};

UnixListenerTest::UnixListenerTest()
{
    _log = sp::Log::getInstance();

    _ipcConfig.bufSize = 4096;
    _ipcConfig.socketPath = _socketPath;

    _unixListener = new sp::UnixListener(_ipcConfig, _gpsReader, _magnetometerReader);
}

UnixListenerTest::~UnixListenerTest()
{
    delete _unixListener;
    sp::Log::release();
}

void UnixListenerTest::SetUp()
{
    _unixListener->start();
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
}

void UnixListenerTest::TearDown()
{
    _unixListener->stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
}

int UnixListenerTest::connectClient()
{
    int sockfd;
    sockaddr_un address;
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        return -1;
    }
    std::memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, _socketPath, sizeof(address.sun_path) - 1);

    if (connect(sockfd, (struct sockaddr *)&address, sizeof(address)) == -1)
    {
        _log->write(sp::LogLevel::ERROR, "UnixListenerTest failed to connect to %s: %d\n", _socketPath, errno);
        close(sockfd);
        return -1;
    }

    return sockfd;
}

TEST_F(UnixListenerTest, GetGPSData)
{
    _log->write(sp::LogLevel::DEBUG, "UnixListenerTest::GetGPSData\n");

    char buf[4096];
    std::memset(reinterpret_cast<void *>(buf), 0, sizeof(buf));

    int sockfd = connectClient();
    if (sockfd == -1)
    {
        FAIL();
    }

    sp::IPCRequest rq;
    rq.cmd = rq.cmdGetGPS;
    json rqJson = rq;
    std::string rqStr = rqJson.dump();

    _log->write(sp::LogLevel::DEBUG, "UnixListenerTest::GetGPSData sending request %s\n", rqStr.c_str());

    if (write(sockfd, rqStr.c_str(), rqStr.length()) == -1)
    {
        _log->write(sp::LogLevel::ERROR, "UnixListenerTest failed to write to client socket: %d\n", errno);
        close(sockfd);
        FAIL();
    }

    int numRead = read(sockfd, reinterpret_cast<void *>(buf), 4096);
    if (numRead == -1)
    {
        _log->write(sp::LogLevel::ERROR, "UnixListenerTest failed to read from client socket: %d\n", errno);
        close(sockfd);
        FAIL();
    }

    _log->write(sp::LogLevel::DEBUG, "UnixListenerTest::GetGPSData received response %s\n", buf);

    json respJson = json::parse(buf);
    sp::GPSInfoResponse resp = respJson.get<sp::GPSInfoResponse>();

    EXPECT_EQ(10, resp.numSatellites);
    EXPECT_EQ(56.34737, resp.latitude);
    EXPECT_EQ(43.98704, resp.longitude);
    EXPECT_EQ(1.0, resp.speedKnots);
    EXPECT_EQ(1.8, resp.speedKm);

    close(sockfd);
}

TEST_F(UnixListenerTest, GetMagnetometerData)
{
    _log->write(sp::LogLevel::DEBUG, "UnixListenerTest::GetMagnetometerData\n");

    char buf[4096];
    std::memset(reinterpret_cast<void *>(buf), 0, sizeof(buf));

    int sockfd = connectClient();
    if (sockfd == -1)
    {
        FAIL();
    }

    sp::IPCRequest rq;
    rq.cmd = rq.cmdGetMagnetometer;
    json rqJson = rq;
    std::string rqStr = rqJson.dump();

    _log->write(sp::LogLevel::DEBUG, "UnixListenerTest::GetMagnetometerData sending request %s\n", rqStr.c_str());

    if (write(sockfd, rqStr.c_str(), rqStr.length()) == -1)
    {
        _log->write(sp::LogLevel::ERROR, "UnixListenerTest failed to write to client socket: %d\n", errno);
        close(sockfd);
        FAIL();
    }

    int numRead = read(sockfd, reinterpret_cast<void *>(buf), 4096);
    if (numRead == -1)
    {
        _log->write(sp::LogLevel::ERROR, "UnixListenerTest failed to read from client socket: %d\n", errno);
        close(sockfd);
        FAIL();
    }

    _log->write(sp::LogLevel::DEBUG, "UnixListenerTest::GetMagnetometerData received response %s\n", buf);

    json respJson = json::parse(buf);
    sp::MagnetometerInfoResponse resp = respJson.get<sp::MagnetometerInfoResponse>();

    EXPECT_EQ(777, resp.x);
    EXPECT_EQ(98639, resp.y);
    EXPECT_EQ(-84, resp.z);

    close(sockfd);
}