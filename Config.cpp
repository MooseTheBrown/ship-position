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

#include "Config.hpp"
#include <fstream>

namespace ship_position
{

Config::Config(const std::string &configFile)
: _ok(false)
{
    _log = Log::getInstance();

    json j;
    std::ifstream in(configFile.c_str());

    if (!in.is_open())
    {
        _log->write(LogLevel::ERROR, "Failed to open config file %s\n", configFile.c_str());
        return;
    }

    in >> j;

    _configData = j.get<ConfigData>();

    _ok = true;
}

LogLevel Config::getLogLevel() const
{
    if (_configData.logLevel == "debug")
    {
        return LogLevel::DEBUG;
    }
    else if (_configData.logLevel == "notice")
    {
        return LogLevel::NOTICE;
    }
    else 
    {
        return LogLevel::ERROR;
    }
}

bool Config::isConsoleLogEnabled() const
{
    for (auto &backend : _configData.logBackends)
    {
        if (backend == "console")
        {
            return true;
        }
    }
}

bool Config::isSyslogEnabled() const
{
    for (auto &backend : _configData.logBackends)
    {
        if (backend == "syslog")
        {
            return true;
        }
    }
}

}