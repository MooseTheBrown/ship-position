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

#include "ShipPosition.hpp"

namespace sp = ship_position;

static sp::ShipPosition *app = nullptr;

int main(int argc, char *argv[])
{
    app = new sp::ShipPosition();
    int ret = app->run(argc, argv);
    delete app;
    return ret;
}

void signal_handler(int sig)
{
    app->stop();
}