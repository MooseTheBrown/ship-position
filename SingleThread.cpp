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

#include "SingleThread.hpp"
#include "Log.hpp"
#include <functional>

namespace ship_position
{

SingleThread::SingleThread()
: _need_to_stop(false),
  _thread(nullptr)
{
}

SingleThread::~SingleThread()
{
}

void SingleThread::start()
{
    if (_thread == nullptr)
    {
        _thread = new std::thread(&SingleThread::run, this);
    }
}

void SingleThread::stop()
{
    if (_thread != nullptr)
    {
        _need_to_stop = true;
        cleanup();
    }
}

void SingleThread::cleanup()
{
    if (_thread != nullptr)
    {
        if (_thread->joinable())
        {
            _thread->join();
        }
        delete _thread;
        _thread = nullptr;
        _need_to_stop = false;
    }
}

}
