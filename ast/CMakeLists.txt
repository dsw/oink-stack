# Copyright 2022, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

bison_target(agrampar agrampar.y agrampar)

add_library(
    ast ${CMAKE_CURRENT_BINARY_DIR}/agrampar.tab.h ${CMAKE_CURRENT_BINARY_DIR}/agrampar.tab.cc
        ${CMAKE_CURRENT_BINARY_DIR}/agrampar.codes.h)
