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

#ifndef SINGLETHREAD_HPP
#define SINGLETHREAD_HPP

#include <thread>

namespace ship_position
{

// class, which manages single thread for execution of some background task
class SingleThread
{
public:
    SingleThread();
    virtual ~SingleThread();

    virtual void start();
    virtual void stop();

    virtual void run() = 0;

protected:
    void cleanup();
    bool need_to_stop() { return _need_to_stop; }

private:
    std::thread *_thread;
    bool _need_to_stop;
};

}

#endif // SINGLETHREAD_HPP
