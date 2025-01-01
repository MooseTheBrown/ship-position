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

#ifndef IPCMESSAGES_HPP
#define IPCMESSAGES_HPP

#include "GPSReader.hpp"
#include "MagnetometerReader.hpp"
#include "json.hpp"
#include <string>

namespace ship_position
{

struct IPCRequest
{
    const std::string cmdGetGPS = "GetGPSData";
    const std::string cmdGetMagnetometer = "GetMagnetometerData";

    std::string cmd;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(IPCRequest, cmd)
};

struct GPSInfoResponse
{
    GPSInfoResponse() = default;

    GPSInfoResponse(const GPSInfo &gpsInfo)
    {
        numSatellites = gpsInfo.numSatellites;
        latitude = gpsInfo.latitude;
        longitude = gpsInfo.longitude;
        speedKnots = gpsInfo.speedKnots;
        speedKm = gpsInfo.speedKm;
    }

    int numSatellites;
    double latitude;
    double longitude;
    double speedKnots;
    double speedKm;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(GPSInfoResponse, numSatellites, latitude, longitude, speedKnots, speedKm)
};

struct MagnetometerInfoResponse
{
    int32_t x;
    int32_t y;
    int32_t z;

    MagnetometerInfoResponse() = default;

    MagnetometerInfoResponse(const MagnetometerData &data)
    {
        x = data.x;
        y = data.y;
        z = data.z;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MagnetometerInfoResponse, x, y, z)
};

struct ErrorResponse
{
    std::string errorMessage;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ErrorResponse, errorMessage)
};

}

#endif // IPCMESSAGES_HPP 