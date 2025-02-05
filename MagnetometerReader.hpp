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

#ifndef MAGNETOMETER_READER_HPP
#define MAGNETOMETER_READER_HPP

#include <cstdint>

namespace ship_position
{

struct MagnetometerData
{
    int32_t x;
    int32_t y;
    int32_t z;

    MagnetometerData() : x(0), y(0), z(0) {}
};

class MagnetometerReader
{
public:
    virtual void getMagnetometerData(MagnetometerData &data) = 0;
    virtual void startCalibration() = 0;
    virtual void stopCalibration() = 0;
};

}

#endif // MAGNETOMETER_READER_HPP