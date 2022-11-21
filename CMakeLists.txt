# Copyright 2022, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0
cmake_minimum_required(VERSION 3.10)
project(oink-stack)

include(CTest)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

find_package(Perl REQUIRED)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# find_package(M4)

set(RUN_FLEX
    ${CMAKE_CURRENT_SOURCE_DIR}/smbase/run-flex.pl
    CACHE INTERNAL "" FORCE)

function(add_wrapped_flex NAME INPUT OUTPUT)
    if(NOT IS_ABSOLUTE ${INPUT})
        set(INPUT ${CMAKE_CURRENT_SOURCE_DIR}/${INPUT})
    endif()
    if(NOT IS_ABSOLUTE ${OUTPUT})
        set(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT})
    endif()
    add_custom_command(
        OUTPUT ${OUTPUT}
        COMMAND ${PERL_EXECUTABLE} ${RUN_FLEX} -nobackup -copies -o${OUTPUT} ${INPUT}
        DEPENDS ${INPUT} ${RUN_FLEX}
        COMMENT "Running run-flex.pl wrapper to generate ${OUTPUT}"
        VERBATIM)
endfunction()

add_subdirectory(smbase)
add_subdirectory(ast)
add_subdirectory(elkhound)
add_subdirectory(elsa)
