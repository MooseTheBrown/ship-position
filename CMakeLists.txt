#
# Copyright (C) 2024 - 2025 Mikhail Sapozhnikov
#
# This file is part of ship-position.
#
# ship-position is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# ship-position is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with ship-position.  If not, see <http://www.gnu.org/licenses/>.
#

cmake_minimum_required (VERSION 3.20)

project (ship-position)

option (BUILD_TESTS "Build tests" OFF)

set (COMMON_CXX_FLAGS "-std=c++23 -pthread")
set (TEST_CXX_FLAGS "")
set (CMAKE_BUILD_TYPE RelWithDebInfo)
set (CMAKE_LINKER_FLAGS -ldl)
set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${COMMON_CXX_FLAGS} ${TEST_CXX_FLAGS} -ggdb")
set (CMAKE_CXX_FLAGS_RELEASE "${COMMON_CXX_FLAGS} ${TEST_CXX_FLAGS}")

set (SHIPPOSITION_SRC BN880GPSReader.cpp
                      Config.cpp
                      IPCClient.cpp
                      NMEAParser.cpp
                      Log.cpp
                      SingleThread.cpp
                      UnixListener.cpp
                      ShipPosition.cpp
                      QMC5883LReader.cpp)

include_directories (${ship-position_SOURCE_DIR})

find_library (BOOST_PO_LIB NAMES boost_program_options)
if (${BOOST_PO_LIB} EQUAL "BOOST_PO_LIB-NOTFOUND")
    message(FATAL_ERROR "boost_program_options not found")
endif (${BOOST_PO_LIB} EQUAL "BOOST_PO_LIB-NOTFOUND")

find_library (I2C_LIB NAMES i2c)
if (${I2C_LIB} EQUAL "I2C_LIB-NOTFOUND")
    message(FATAL_ERROR "i2c not found")
endif (${I2C_LIB} EQUAL "I2C_LIB-NOTFOUND")

add_executable (ship-position ${SHIPPOSITION_SRC} main.cpp)
target_link_libraries (ship-position ${BOOST_PO_LIB} ${I2C_LIB})

install (TARGETS ship-position DESTINATION bin)
install (FILES ship-position.conf DESTINATION /etc)

if (BUILD_TESTS)
    set (TESTS_SRC ${SHIPPOSITION_SRC}
                   test/main.cpp
                   test/NMEAParser_test.cpp
                   test/Config_test.cpp
                   test/UnixListener_test.cpp)
    find_library (GTEST_LIB NAMES gtest)
    if (${GTEST_LIB} EQUAL "GTEST_LIB-NOTFOUND")
        message(FATAL_ERROR "Google Test not found")
    endif (${GTEST_LIB} EQUAL "GTEST_LIB-NOTFOUND")
    configure_file(test/testconfig.conf testconfig.conf COPYONLY)
    add_executable (ship-position-test ${TESTS_SRC})
    target_link_libraries (ship-position-test ${GTEST_LIB} ${BOOST_PO_LIB} ${I2C_LIB})
endif (BUILD_TESTS)