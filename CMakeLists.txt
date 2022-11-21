# Copyright 2022, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0
cmake_minimum_required(VERSION 3.10)
project(oink-stack)

include(CTest)

find_package(BISON REQUIRED)

# find_package(M4)

add_subdirectory(smbase)
