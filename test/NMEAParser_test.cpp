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
#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace sp = ship_position;

class NMEAParserAdapter : public sp::NMEAParser
{
public:
    void split(const std::string &data, const std::string &delimiter, std::vector<std::string> &tokens)
    {
        NMEAParser::split(data, delimiter, tokens);
    }
};

TEST(NMEAParser, Split_Simple)
{
    NMEAParserAdapter parserAdapter;
    std::string data = "text,which,needs,to,be,separated";
    std::string delimiter = ",";
    std::vector<std::string> expected = {"text", "which", "needs", "to", "be", "separated"};
    std::vector<std::string> actual;
    parserAdapter.split(data, delimiter, actual);
    ASSERT_EQ(expected, actual);
}

TEST(NMEAParser, Split_NoDelimiter)
{
    NMEAParserAdapter parserAdapter;
    std::string data = "text";
    std::string delimiter = ",";
    std::vector<std::string> expected = {"text"};
    std::vector<std::string> actual;
    parserAdapter.split(data, delimiter, actual);
    ASSERT_EQ(expected, actual);
}

TEST(NMEAParser, Split_EmptyData)
{
    NMEAParserAdapter parserAdapter;
    std::string data = "";
    std::string delimiter = ",";
    std::vector<std::string> expected = {};
    std::vector<std::string> actual;
    parserAdapter.split(data, delimiter, actual);
    ASSERT_EQ(expected, actual);
}

TEST(NMEAParser, Parse_NorthEast)
{
    sp::NMEAParser parser;
    std::string data = R"($GNRMC,170257.00,A,5619.06488,N,04401.12281,E,0.071,,300324,,,A*68
$GNVTG,,T,,M,0.071,N,0.131,K,A*38
$GNGGA,170257.00,5619.06488,N,04401.12281,E,1,09,1.36,124.2,M,6.3,M,,*79
$GNGSA,A,3,02,23,10,14,22,32,21,,,,,,2.67,1.36,2.29*17
$GNGSA,A,3,78,85,,,,,,,,,,,2.67,1.36,2.29*10
$GPGSV,3,1,09,02,27,297,26,10,72,079,34,14,11,333,27,18,00,120,*79
$GPGSV,3,2,09,21,44,293,31,22,07,352,27,23,33,074,36,24,19,052,*70
$GPGSV,3,3,09,32,50,156,28*4C
$GLGSV,3,1,10,69,12,039,,70,75,070,17,71,46,206,22,72,01,214,*67
$GLGSV,3,2,10,77,04,303,,78,17,357,30,79,07,045,,85,48,146,18*69
$GLGSV,3,3,10,86,74,300,23,87,20,314,*60
$GNGLL,5619.06488,N,04401.12281,E,170257.00,A,A*71
)";
    sp::GPSInfo gpsInfo;
    parser.parse(data, gpsInfo);
    ASSERT_EQ(9, gpsInfo.numSatellites);
    ASSERT_EQ(56.317748, gpsInfo.latitude);
    ASSERT_EQ(44.0187135, gpsInfo.longitude);
    ASSERT_EQ(0.071, gpsInfo.speedKnots);
    ASSERT_EQ(0.131, gpsInfo.speedKm);
}

TEST(NMEAParser, Parse_NorthWest)
{
    sp::NMEAParser parser;
    std::string data = R"($GNRMC,170257.00,A,5619.06488,N,04401.12281,E,0.071,,300324,,,A*68
$GNGGA,170259.00,5619.06488,N,04401.12281,W,1,09,1.36,124.2,M,6.3,M,,*79
$GLGSV,3,2,10,77,04,303,,78,17,357,30,79,07,045,,85,48,146,18*69
$GLGSV,3,3,10,86,74,300,23,87,20,314,*60
$GNGLL,5619.06488,N,04401.12281,E,170257.00,A,A*71
)";
    sp::GPSInfo gpsInfo;
    parser.parse(data, gpsInfo);
    ASSERT_EQ(56.317748, gpsInfo.latitude);
    ASSERT_EQ(-44.0187135, gpsInfo.longitude);
}

TEST(NMEAParser, Parse_SouthWest)
{
    sp::NMEAParser parser;
    std::string data = R"($GNRMC,170257.00,A,5619.06488,N,04401.12281,E,0.071,,300324,,,A*68
$GNGGA,170259.00,5619.06488,S,04401.12281,W,1,09,1.36,124.2,M,6.3,M,,*79
$GLGSV,3,2,10,77,04,303,,78,17,357,30,79,07,045,,85,48,146,18*69
$GLGSV,3,3,10,86,74,300,23,87,20,314,*60
$GNGLL,5619.06488,N,04401.12281,E,170257.00,A,A*71
)";
    sp::GPSInfo gpsInfo;
    parser.parse(data, gpsInfo);
    ASSERT_EQ(-56.317748, gpsInfo.latitude);
    ASSERT_EQ(-44.0187135, gpsInfo.longitude);
}

TEST(NMEAParser, Incomplete_Sentence)
{
    sp::NMEAParser parser;
    std::string data = R"($GNRMC,170257.00,A,5619.06488,N,04401.12281,E,0.071,,300324,,,A*68
$GNGGA,170259.00,5619.06488,N,04401.12281)";
    sp::GPSInfo gpsInfo;
    parser.parse(data, gpsInfo);
    ASSERT_EQ(0, gpsInfo.latitude);
    ASSERT_EQ(0, gpsInfo.longitude);
    ASSERT_EQ(0, gpsInfo.numSatellites);
    ASSERT_EQ(0, gpsInfo.speedKm);
    ASSERT_EQ(0, gpsInfo.speedKnots);
}