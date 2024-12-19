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

#ifndef HMC5883LREADER_HPP
#define HMC5883LREADER_HPP

#include "SingleThread.hpp"
#include "Log.hpp"

namespace ship_position
{

class HMC5883LReader : public SingleThread
{
public:
    HMC5883LReader();
    virtual ~HMC5883LReader();
    virtual void run();
protected:
    void init();
    int findDevice(const std::string &adapterName, std::string &deviceName);

    int _fd;
    Log *_log;
};

}

#endif // HMC5883LREADER_HPP