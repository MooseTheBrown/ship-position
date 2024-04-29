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

#ifndef GPSREADER_HPP
#define GPSREADER_HPP

namespace ship_position
{

struct GPSInfo
{
    int numSatellites;
    double latitude;
    double longitude;
    double speedKnots;
    double speedKm;

    GPSInfo()
    {
        numSatellites = 0;
        latitude = 0.0;
        longitude = 0.0;
        speedKnots = 0.0;
        speedKm = 0.0;
    }
};

class GPSReader
{
public:
    virtual void getGPSInfo(GPSInfo &gpsInfo) = 0;
};

}

#endif // GPSREADER_HPP