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

#ifndef QMC5883LREADER_HPP
#define QMC5883LREADER_HPP

#include "SingleThread.hpp"
#include "Log.hpp"
#include "QMC5883LConfig.hpp"
#include "MagnetometerReader.hpp"

#include <cstdint>
#include <shared_mutex>

#define QMC5883L_I2C_ADDR 0x0D

namespace ship_position
{

class QMC5883LReader : public SingleThread, public MagnetometerReader
{
public:
    QMC5883LReader(const QMC5883LConfig &config);
    virtual ~QMC5883LReader();
    virtual void run();
    virtual void GetMagnetometerData(MagnetometerData &data);
protected:
    void init();
    int32_t readWord2C(uint8_t lowReg, uint8_t highReg, int32_t &res);

    const QMC5883LConfig &_config;
    int _fd;
    Log *_log;
    MagnetometerData _magnetometerData;
    std::shared_mutex _magnetometerDataMutex;
};

}

#endif // QMC5883LREADER_HPP