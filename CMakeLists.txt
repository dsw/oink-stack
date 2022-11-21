# Copyright 2022, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0
cmake_minimum_required(VERSION 3.10)
project(oink-stack)

include(CTest)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# find_package(M4)

add_subdirectory(smbase)
add_subdirectory(ast)
add_subdirectory(elkhound)
add_subdirectory(elsa)
