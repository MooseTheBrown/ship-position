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

#ifndef NMEA_PARSER_HPP
#define NMEA_PARSER_HPP

#include "GPSReader.hpp"
#include <string>
#include <vector>

namespace ship_position
{

class NMEAParser
{
public:
    void parse(const std::string &data, GPSInfo &gpsInfo);

protected:
    void split(const std::string &data, const std::string &delimited, std::vector<std::string> &tokens);
    void parseGGA(const std::vector<std::string> &fields, GPSInfo &gpsInfo);
    void parseVTG(const std::vector<std::string> &fields, GPSInfo &gpsInfo);
    double parseCoordinates(const std::string &digits, const std::string &direction);
};

}

#endif // NMEA_PARSER_HPP