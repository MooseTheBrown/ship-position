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

#ifndef BN880GPSREADER_HPP
#define BN880GPSREADER_HPP

#include "SingleThread.hpp"
#include "BN880GPSConfig.hpp"
#include "GPSReader.hpp"
#include "Log.hpp"
#include "NMEAParser.hpp"
#include <shared_mutex>

namespace ship_position
{

class BN880GPSReader : public SingleThread, public GPSReader
{
public:
    BN880GPSReader(const BN880GPSConfig &config);
    virtual ~BN880GPSReader();

    virtual void run();

    virtual void getGPSInfo(GPSInfo &gpsInfo);
protected:
    void init(const std::string &devPath);
    void setupRawOutput(const std::string &rawOutputPath);
    void writeRawOutput(const std::string &rawData);

    const BN880GPSConfig &_config;
    int _fd;
    int _rawfd;
    Log *_log;
    char *_readbuf;
    int _readErrors;
    GPSInfo _gpsInfo;
    std::shared_mutex _gpsInfoMutex;
};

}

#endif // BN880GPSREADER_HPP