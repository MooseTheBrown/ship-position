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

#include "NMEAParser.hpp"

namespace ship_position
{

void NMEAParser::parse(const std::string &data, GPSInfo &gpsInfo)
{
    std::vector<std::string> lines;
    split(data, "\n", lines);
    for (auto& line : lines)
    {
        std::vector<std::string> fields;
        split(line, ",", fields);

        if (fields.empty())
            continue;

        std::string key = fields[0];

        if (key.find("GGA") != std::string::npos)
        {
            parseGGA(fields, gpsInfo);
        }
        else if (key.find("VTG") != std::string::npos)
        {
            parseVTG(fields, gpsInfo);
        }
    }   
}

void NMEAParser::parseGGA(const std::vector<std::string> &fields, GPSInfo &gpsInfo)
{
    if (fields.size() < 8)
    {
        return;
    }

    if (!fields[2].empty() && !fields[3].empty())
    {
        gpsInfo.latitude = parseCoordinates(fields[2], fields[3]);
    }
    if (!fields[4].empty() && !fields[5].empty())
    {
        gpsInfo.longitude = parseCoordinates(fields[4], fields[5]);
    }
    if (!fields[7].empty())
    {
        gpsInfo.numSatellites = std::stoi(fields[7]);
    }
}

void NMEAParser::parseVTG(const std::vector<std::string> &fields, GPSInfo &gpsInfo)
{
    if (fields.size() < 8)
    {
        return;
    }

    if (!fields[5].empty())
    {
        gpsInfo.speedKnots = std::stod(fields[5]);
    }
    if (!fields[7].empty())
    {
        gpsInfo.speedKm = std::stod(fields[7]);
    }
}

void NMEAParser::split(const std::string &data, const std::string &delimiter, std::vector<std::string> &tokens)
{
    size_t pos = 0;

    if (data.empty())
    {
        return;
    }

    if ((pos = data.find(delimiter)) != std::string::npos)
    {
        tokens.push_back(data.substr(0, pos));
        split(data.substr(pos + 1), delimiter, tokens);
    }
    else
    {
        tokens.push_back(data);
    }
}

double NMEAParser::parseCoordinates(const std::string &digits, const std::string &direction)
{
    std::string trimmedDigits = digits.substr(digits.find_first_not_of("0"));
    double coordinates = std::stod(trimmedDigits.substr(0, 2)) + (std::stod(trimmedDigits.substr(2)) / 60);
    if ((direction == "S") || (direction == "W"))
    {
        coordinates *= -1;
    }

    return coordinates;
}

}