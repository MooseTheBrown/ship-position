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

#include "Config.hpp"
#include <gtest/gtest.h>

namespace sp = ship_position;

TEST(Config, ConfigTest)
{
    sp::Config config("./testconfig.conf");

    ASSERT_TRUE(config.isOk());

    sp::BN880GPSConfig gpsConfig;
    config.getBN880GPSConfig(gpsConfig);

    ASSERT_EQ("/dev/testdevice", gpsConfig.devPath);
    ASSERT_EQ(4096, gpsConfig.bufferSize);
    ASSERT_EQ(3, gpsConfig.maxRetries);
    ASSERT_EQ("/var/run/rawgps.log", gpsConfig.rawOutput);
    ASSERT_EQ(1073741824, gpsConfig.maxRawFileSize);

    sp::QMC5883LConfig qmcConfig;
    config.getQMC5883LConfig(qmcConfig);

    ASSERT_EQ("/dev/i2c-99", qmcConfig.devPath);
    ASSERT_EQ(274, qmcConfig.pollTimeout);
    ASSERT_EQ(111, qmcConfig.calibrationPollTimeout);

    sp::IPCConfig ipcConfig;
    config.getIPCConfig(ipcConfig);

    ASSERT_EQ(5120, ipcConfig.bufSize);
    ASSERT_EQ("/tmp/ship_position.sock", ipcConfig.socketPath);

    ASSERT_EQ(sp::LogLevel::DEBUG, config.getLogLevel());
    ASSERT_TRUE(config.isConsoleLogEnabled());
    ASSERT_TRUE(config.isSyslogEnabled());
}