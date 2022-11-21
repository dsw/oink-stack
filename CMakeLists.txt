# Copyright 2022, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0
cmake_minimum_required(VERSION 3.10)
project(oink-stack)

include(CTest)
include(GNUInstallDirs)

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

find_package(Perl REQUIRED)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# find_package(M4)

set(RUN_FLEX
    ${CMAKE_CURRENT_SOURCE_DIR}/smbase/run-flex.pl
    CACHE INTERNAL "" FORCE)

# Prefer normal flex_target if possible, but if it's not...
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

# Wrap adding a custom command for running astgen
function(add_astgen_command)
    set(options)
    set(oneValueArgs OUTPUT_PREFIX)
    set(multiValueArgs INPUTS OUTPUTS TRANSLATE_OPTIONS)
    cmake_parse_arguments(
        _AST
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN})
    if(_AST_TRANSLATE_OPTIONS)
        string(REPLACE ";" "," _AST_TRANSLATE_OPTIONS "${_AST_TRANSLATE_OPTIONS}")
        set(_AST_TRANSLATE_OPTIONS -tr ${_AST_TRANSLATE_OPTIONS})
    endif()
    add_custom_command(
        OUTPUT ${_AST_OUTPUTS}
        COMMAND ${CMAKE_COMMAND} -E rm -f ${_AST_OUTPUTS}
        COMMAND astgen ${_AST_TRANSLATE_OPTIONS} -o${_AST_OUTPUT_PREFIX} ${_AST_INPUTS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS astgen ${_AST_INPUTS}
        COMMENT
            "Running astgen on ${_AST_INPUTS} to generate outputs prefixed by ${_AST_OUTPUT_PREFIX}"
        VERBATIM)
endfunction()

# Wrap adding a custom command for running elkhound
function(add_elkhound_command)
    set(options)
    set(oneValueArgs OUTPUT_PREFIX CC_OUTPUT_VAR WORKING_DIRECTORY)
    set(multiValueArgs INPUTS TRANSLATE_OPTIONS EXTRA_DEPENDS)
    cmake_parse_arguments(
        _ELK
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN})
    if(_ELK_TRANSLATE_OPTIONS)
        string(REPLACE ";" "," _ELK_TRANSLATE_OPTIONS "${_ELK_TRANSLATE_OPTIONS}")
        set(_ELK_TRANSLATE_OPTIONS -tr ${_ELK_TRANSLATE_OPTIONS})
    endif()
    if(NOT _ELK_WORKING_DIRECTORY)
        set(_ELK_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    endif()
    set(CC_OUTPUTS "${_ELK_OUTPUT_PREFIX}.cc" "${_ELK_OUTPUT_PREFIX}.h")
    set(Y_OUTPUTS "${_ELK_OUTPUT_PREFIX}.y")
    set(LOG_OUTPUTS "${_ELK_OUTPUT_PREFIX}.out")
    add_custom_command(
        OUTPUT ${CC_OUTPUTS} ${Y_OUTPUTS} ${LOG_OUTPUTS}
        COMMAND ${CMAKE_COMMAND} -E rm -f ${CC_OUTPUTS} ${Y_OUTPUTS} ${LOG_OUTPUTS}
        COMMAND elkhound ${_ELK_TRANSLATE_OPTIONS} -o ${_ELK_OUTPUT_PREFIX} ${_ELK_INPUTS}
        WORKING_DIRECTORY ${_ELK_WORKING_DIRECTORY}
        DEPENDS elkhound ${_ELK_INPUTS} ${_ELK_EXTRA_DEPENDS}
        COMMENT
            "Running elkhound on ${_ELK_INPUTS} to generate outputs prefixed by ${_ELK_OUTPUT_PREFIX}"
        VERBATIM)
    if(_ELK_CC_OUTPUT_VAR)
        set(${_ELK_CC_OUTPUT_VAR}
            ${CC_OUTPUTS}
            PARENT_SCOPE)
    endif()
endfunction()

add_subdirectory(smbase)
add_subdirectory(ast)
add_subdirectory(elkhound)
add_subdirectory(elsa)
