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

#include "QMC5883LReader.hpp"

extern "C"
{
    #include <linux/i2c-dev.h>
    #include <i2c/smbus.h>
}

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace ship_position
{

QMC5883LReader::QMC5883LReader(const QMC5883LConfig &config) :
_config(config),
_fd(-1),
_calibrating(false)
{
    _log = Log::getInstance();
    _calibration.xmin = 0;
    _calibration.xmax = 0;
    _calibration.ymin = 0;
    _calibration.ymax = 0;
    _calibration.zmax = 0;
    _calibration.zmin = 0;

    init();
}

QMC5883LReader::~QMC5883LReader()
{
    Log::release();
    
    if (_fd != -1)
    {
        close(_fd);
    }
}

void QMC5883LReader::init()
{
    _log->write(LogLevel::DEBUG, "QMC5883LReader::init(), device: %s\n", _config.devPath.c_str());

    _fd = open(_config.devPath.c_str(), O_RDWR);
    if (_fd == -1)
    {
        _log->write(LogLevel::ERROR, "error opening qmc5883l device %s, error = %d\n",
            _config.devPath.c_str(), errno);
        return;
    }

    if (ioctl(_fd, I2C_SLAVE, QMC5883L_I2C_ADDR) == -1)
    {
        _log->write(LogLevel::ERROR, "error specifying i2c address to communicate with, error = %d\n",
            errno);
        return;
    }

    // define set/reset period
    if (i2c_smbus_write_byte_data(_fd, 0x0D, 0x01) == -1)
    {
        _log->write(LogLevel::ERROR, "failed to set qmc5883l register 0x0B to 0x01, error = %d\n", errno);
        return;
    }

    // set control register: continuous mode, 10 Hz ODR, 2G field range, 512 OSR
    if (i2c_smbus_write_byte_data(_fd, 0x09, 0x01) == -1)
    {
        _log->write(LogLevel::ERROR, "failed to set qmc5883l register 0x09 to 0x01, error = %d\n", errno);
        return;
    }
}

void QMC5883LReader::run()
{
    _log->write(LogLevel::DEBUG, "QMC5883LReader::run()\n");

    if (_fd == -1)
    {
        _log->write(LogLevel::ERROR, "qmc5883l device not initialized, run() quitting\n");
        return;
    }

    while (true)
    {
        bool calibrating = _calibrating;

        // read status register to check whether data is ready
        __s32 res = i2c_smbus_read_byte_data(_fd, 0x06);
        if (res == -1)
        {
            _log->write(LogLevel::ERROR, "failed to read qmc5883l status register 0x06, error = %d\n", errno);
            std::this_thread::sleep_for(std::chrono::milliseconds(_config.pollTimeout));
            continue;
        }

        if (res & 0x01)
        {
            int32_t x = 0;
            int32_t y = 0;
            int32_t z = 0;

            if (readWord2C(0x00, 0x01, x) == -1)
            {
                _log->write(LogLevel::ERROR, "failed to read qmc5883l x axis\n");
            }
            if (readWord2C(0x02, 0x03, y) == -1)
            {
                _log->write(LogLevel::ERROR, "failed to read qmc5883l y axis\n");
            }
            if (readWord2C(0x04, 0x05, z) == -1)
            {
                _log->write(LogLevel::ERROR, "failed to read qmc5883l z axis\n");
            }

            if (calibrating)
            {
                if ((x < _calibration.xmin) || (_calibration.xmin == 0))
                {
                    _calibration.xmin = x;
                }
                else if ((x > _calibration.xmax) || (_calibration.xmax == 0))
                {
                    _calibration.xmax = x;
                }
                if ((y < _calibration.ymin) || (_calibration.xmin == 0))
                {
                    _calibration.ymin = y;
                }
                else if ((y > _calibration.ymax) || (_calibration.ymax == 0))
                {
                    _calibration.ymax = y;
                }
                if ((z < _calibration.zmin) || (_calibration.zmin == 0))
                {
                    _calibration.zmin = z;
                }
                else if ((z > _calibration.zmax) || (_calibration.zmax == 0))
                {
                    _calibration.zmax = z;
                }
            }
            else 
            {
                std::unique_lock<std::shared_mutex> lock(_magnetometerDataMutex);
                _magnetometerData.x = x - ((_calibration.xmin + _calibration.xmax) / 2);
                _magnetometerData.y = y - ((_calibration.ymin + _calibration.ymax) / 2);
                _magnetometerData.z = z - ((_calibration.zmin + _calibration.zmax) / 2);
            }
        }
        else
        {
            _log->write(LogLevel::DEBUG, "qmc5883l status register = 0x%02x, skipping measurement\n", res);
        }

        if (calibrating)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(_config.calibrationPollTimeout));
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(_config.pollTimeout));
        }
    }
}

int QMC5883LReader::readWord2C(uint8_t lowReg, uint8_t highReg, int32_t &res)
{
    __s32 low = i2c_smbus_read_byte_data(_fd, lowReg);
    if (low == -1)
    {
        _log->write(LogLevel::ERROR, "failed to read qmc5883l register 0x%02x, error = %d\n", lowReg, errno);
        return -1;
    }

    __s32 high = i2c_smbus_read_byte_data(_fd, highReg);
    if (high == -1)
    {
        _log->write(LogLevel::ERROR, "failed to read qmc5883l register 0x%02x, error = %d\n", highReg, errno);
        return -1;
    }

    res = (high << 8) + low;
    if (res >= 0x08000)
    {
        res -= 0x10000;
    }

    return 0;
}

void QMC5883LReader::getMagnetometerData(MagnetometerData &data)
{
    std::shared_lock<std::shared_mutex> lock(_magnetometerDataMutex);
    data = _magnetometerData;
}

void QMC5883LReader::startCalibration()
{
    _calibrating = true;
    _log->write(LogLevel::DEBUG, "qmc5883L calibration started\n");
}

void QMC5883LReader::stopCalibration()
{
    _calibrating = false;
    _log->write(LogLevel::DEBUG, "qmc5883L calibration stopped\n");
    _log->write(LogLevel::DEBUG, "qmc5883l calibration data: xmax=%d, xmin=%d, ymax=%d, ymin=%d, zmax=%d, zmin=%d\n",
        _calibration.xmax, _calibration.xmin, _calibration.ymax, _calibration.ymin, _calibration.zmax, _calibration.zmin);
}

}