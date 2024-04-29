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

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "BN880GPSConfig.hpp"
#include "IPCConfig.hpp"
#include "Log.hpp"
#include "json.hpp"

using json = nlohmann::json;

namespace ship_position
{

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BN880GPSConfig, bufferSize, devPath, maxRetries, rawOutput)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(IPCConfig, bufSize, socketPath)

class Config
{
public:
    Config(const std::string &configFile);

    void getBN880GPSConfig(BN880GPSConfig &config) const { config = _configData.bn880GPSConfig; }
    void getIPCConfig(IPCConfig &config) const { config = _configData.ipcConfig;}
    bool isOk() const { return _ok; }

protected:
    struct ConfigData
    {
        BN880GPSConfig bn880GPSConfig;
        IPCConfig ipcConfig;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(ConfigData, bn880GPSConfig, ipcConfig)
    };

    ConfigData _configData;
    Log *_log;
    bool _ok;
};

}

#endif // CONFIG_HPP