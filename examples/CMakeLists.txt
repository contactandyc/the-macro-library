# SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
# SPDX-FileCopyrightText: 2024–2025 Knode.ai
# SPDX-License-Identifier: Apache-2.0
#
# Maintainer: Andy Curtis <contactandyc@gmail.com>

cmake_minimum_required(VERSION 3.10)
project(Examples)

if(DEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I${CMAKE_SOURCE_DIR}/../include -g -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${CMAKE_SOURCE_DIR}/../include -g -O0")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I${CMAKE_SOURCE_DIR}/../include -O3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${CMAKE_SOURCE_DIR}/../include -O3")
endif()

if(ADDRESS_SANITIZER)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif()

add_subdirectory(demo)
add_subdirectory(speed-test)
