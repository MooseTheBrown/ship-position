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

#include "HMC5883LReader.hpp"

namespace ship_position
{

HMC5883LReader::HMC5883LReader()
{
    _log = Log::getInstance();
}

HMC5883LReader::~HMC5883LReader()
{
    Log::release();
}

void HMC5883LReader::init()
{
    std::string deviceName;
    // TODO: insert proper adapter name
    if (findDevice("hmc5883l", deviceName) != 0)
    {
        _log->write(LogLevel::ERROR, "Failed to find HMC5883L i2c device\n");
        return;
    }
}

int HMC5883LReader::findDevice(const std::string &adapterName, std::string &deviceName)
{
    // TODO: scan /sys/class/i2c-adapter to find adapter with given name
    return 0;
}

}