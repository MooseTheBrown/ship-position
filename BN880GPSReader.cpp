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

#include "BN880GPSReader.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <exception>

namespace ship_position
{

BN880GPSReader::BN880GPSReader(const BN880GPSConfig &config) :
    _readErrors(0),
    _config(config),
    _fd(-1),
    _rawfd(-1)
{
    _log = Log::getInstance();
    _log->write(LogLevel::DEBUG, "BN880GPSReader ctor\n");
    _readbuf = new char[config.bufferSize];
    init(config.devPath);
    if (config.rawOutput != "")
    {
        setupRawOutput(config.rawOutput);
    }
}

BN880GPSReader::~BN880GPSReader()
{
    if (_fd != -1)
    {
        close(_fd);
    }
    delete _readbuf;

    if (_rawfd!= -1)
    {
        close(_rawfd);
    }

    Log::release();
}

void BN880GPSReader::init(const std::string &devPath)
{
    _log->write(LogLevel::DEBUG, "BN880GPSReader::init(), devPath=%s\n", devPath.c_str());

    _fd = open(devPath.c_str(), O_RDWR, 0);

    if (_fd != -1)
    {
        struct termios options;
        tcgetattr(_fd, &options);
        options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
        options.c_oflag &= ~(ONLCR | OCRNL);
        tcsetattr(_fd, TCSANOW, &options);
    }
    else
    {
        _log->write(LogLevel::ERROR, "failed to open bn880 gps device %s, error=%d\n", devPath.c_str(), errno);
    }
}

void BN880GPSReader::run()
{
    _log->write(LogLevel::DEBUG, "BN880GPSReader::run()\n");

    if (_fd == -1)
    {
        _log->write(LogLevel::ERROR, "bn880 gps device not initialized, run() quitting\n");
        return;
    }

    NMEAParser nmeaParser;

    while (true)
    {
        if (need_to_stop())
        {
            _log->write(LogLevel::DEBUG, "BN880GPSReader::run() stopping\n");
            break;
        }

        int numRead = read(_fd, _readbuf, _config.bufferSize);
        if (numRead == -1)
        {
            _log->write(LogLevel::ERROR, "failed to read from bn880 gps device, error=%d\n", errno);
            _readErrors++;
            if ((_config.maxRetries != 0) && (_readErrors == _config.maxRetries))
            {
                _log->write(LogLevel::ERROR, "bn880gps reader: too many read errors, quitting\n");
                return;
            }
            continue;
        }
        else
        {
            _log->write(LogLevel::DEBUG, "read %d bytes from bn880 gps device\n", numRead);
            _readErrors = 0;
            std::string nmeaData(_readbuf, numRead);
            std::unique_lock<std::shared_mutex> lock(_gpsInfoMutex);
            nmeaParser.parse(nmeaData, _gpsInfo);
            writeRawOutput(nmeaData);
        }
    }
}

void BN880GPSReader::getGPSInfo(GPSInfo &gpsInfo)
{
    std::shared_lock<std::shared_mutex> lock(_gpsInfoMutex);
    gpsInfo = _gpsInfo;
}

void BN880GPSReader::setupRawOutput(const std::string &rawOutputPath)
{
    _rawfd = open(rawOutputPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0664);
    if (_rawfd == -1)
    {
        _log->write(LogLevel::ERROR, "failed to open raw output file %s, error=%d\n", rawOutputPath.c_str(), errno);
    }
}

void BN880GPSReader::writeRawOutput(const std::string &rawData)
{
    if (_rawfd != -1)
    {
        // check raw output file size and truncate if necessary
        if (_config.maxRawFileSize != 0)
        {
            struct stat st;
            if (fstat(_rawfd, &st) == -1)
            {
                _log->write(LogLevel::ERROR, "BN880GPSReader failed to stat raw output file, error=%d\n", errno);
                return;
            }
            if (st.st_size > _config.maxRawFileSize)
            {
                _log->write(LogLevel::DEBUG, "BN880GPSReader: raw output file size exceeded, truncating\n");
                if (ftruncate(_rawfd, 0) == -1)
                {
                    _log->write(LogLevel::ERROR, "BN880GPSReader failed to truncate raw output file, error=%d\n", errno);
                    return;
                }
            }
        }

        if (write(_rawfd, rawData.c_str(), rawData.length()) == -1)
        {
            _log->write(LogLevel::ERROR, "BN880GPSReader failed to write to raw output file, error=%d\n", errno);
        }
    }
}

}
