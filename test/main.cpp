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

#include "Log.hpp"
#include <gtest/gtest.h>

namespace sp = ship_position;

int main(int argc, char *argv[])
{
    sp::ConsoleLog clog;
    sp::Log *log = sp::Log::getInstance();
    log->set_level(sp::LogLevel::DEBUG);
    log->add_backend(&clog);

    // run the tests
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();

    sp::Log::release();
}