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

#ifndef LOG_HPP
#define LOG_HPP

#include <mutex>
#include <vector>
#include <cstdarg>
#include <cstdio>
#include <syslog.h>

namespace ship_position
{

// log backend interface
class LogBackend
{
public:
    virtual ~LogBackend() {}
    virtual void write(const char *fmt, va_list args) = 0;
};

enum class LogLevel : unsigned char
{
    ERROR = 2,
    NOTICE = 1,
    DEBUG = 0
};

// log interface
class Log
{
public:
    static Log *getInstance()
    {
        std::lock_guard<std::mutex> lock(_ref_mutex);

        if (_refs == 0)
        {
            _instance = new Log();
        }
        _refs++;

        return _instance;
    }

    static void release()
    {
        std::lock_guard<std::mutex> lock(_ref_mutex);

        _refs--;
        if (_refs == 0)
        {
            delete _instance;
            _instance = nullptr;
        }
    }

    void add_backend(LogBackend *backend)
    {
        if (backend != nullptr)
        {
            _backends.push_back(backend);
        }
    }

    void write(LogLevel level, const char *fmt, ...)
    {
        if (level >= _level)
        {
            for (LogBackend *backend : _backends)
            {
                va_list args;
                va_start(args, fmt);
                backend->write(fmt, args);
                va_end(args);
            }
        }
    }

    // write message with default log level - notice
    void write(const char *fmt, ...)
    {
        if (LogLevel::NOTICE >= _level)
        {
            for (LogBackend *backend : _backends)
            {
                va_list args;
                va_start(args, fmt);
                backend->write(fmt, args);
                va_end(args);
            }
        }
    }

    void set_level(LogLevel level) { _level = level; }
    LogLevel get_level() const { return _level; }

private:
    Log() : _level(LogLevel::NOTICE) {}
    virtual ~Log() {}

    static unsigned int _refs;
    static std::mutex _ref_mutex;
    static Log *_instance;

    std::vector<LogBackend *> _backends;
    LogLevel _level;
};


// log backend, which writes to syslog
class SysLog : public LogBackend
{
public:
    SysLog()
    {
        openlog("ship-position", LOG_CONS, LOG_USER);
    }

    virtual ~SysLog()
    {
        closelog();
    }

    virtual void write(const char *fmt, va_list args)
    {
        vsyslog(LOG_INFO, fmt, args);
    }
};

// log backend, which writes to console
class ConsoleLog : public LogBackend
{
public:
    ConsoleLog() {}
    virtual ~ConsoleLog() {}

    virtual void write(const char *fmt, va_list args)
    {
        std::vprintf(fmt, args);
    }
};

}

#endif // LOG_HPP