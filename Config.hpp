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

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "BN880GPSConfig.hpp"
#include "IPCConfig.hpp"
#include "QMC5883LConfig.hpp"
#include "Log.hpp"
#include "json.hpp"

using json = nlohmann::json;

namespace ship_position
{

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BN880GPSConfig, bufferSize, devPath, maxRetries, rawOutput, maxRawFileSize)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(QMC5883LConfig, devPath, pollTimeout)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(IPCConfig, bufSize, socketPath)

class Config
{
public:
    Config(const std::string &configFile);

    void getBN880GPSConfig(BN880GPSConfig &config) const { config = _configData.bn880GPSConfig; }
    void getQMC5883LConfig(QMC5883LConfig &config) const { config = _configData.qmc5883LConfig; }
    void getIPCConfig(IPCConfig &config) const { config = _configData.ipcConfig;}
    LogLevel getLogLevel() const;
    bool isSyslogEnabled() const;
    bool isConsoleLogEnabled() const;
    bool isOk() const { return _ok; }

protected:
    struct ConfigData
    {
        BN880GPSConfig bn880GPSConfig;
        QMC5883LConfig qmc5883LConfig;
        IPCConfig ipcConfig;
        std::string logLevel;
        std::vector<std::string> logBackends;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(ConfigData, bn880GPSConfig, qmc5883LConfig, ipcConfig, logLevel, logBackends)
    };

    ConfigData _configData;
    Log *_log;
    bool _ok;
};

}

#endif // CONFIG_HPP