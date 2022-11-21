# Copyright 2022, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

bison_target(
    agrampar agrampar.y ${CMAKE_CURRENT_BINARY_DIR}/agrampar.tab.cc
    COMPILE_FLAGS "--token-table" # -Dapi.prefix={agrampar}"
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/agrampar.tab.h)

flex_target(agramlex agramlex.lex ${CMAKE_CURRENT_BINARY_DIR}/agramlex.yy.cc)
add_flex_bison_dependency(agramlex agrampar)

add_executable(
    ccsstr
    ccsstr.cc
    ccsstr.h
    reporterr.cc
    reporterr.h
    embedded.cc
    embedded.h)
target_compile_definitions(ccsstr PRIVATE TEST_CCSSTR)
target_link_libraries(ccsstr PRIVATE smbase)
target_include_directories(ccsstr PUBLIC ${CMAKE_CURRENT_BINARY_DIR} . ../smbase)

add_library(
    ast
    asthelp.cc
    asthelp.h
    ccsstr.cc
    ccsstr.h
    embedded.cc
    embedded.h
    locstr.cc
    locstr.h
    gramlex.cc
    gramlex.h
    reporterr.cc
    reporterr.h
    xmlhelp.cc
    xmlhelp.h)

target_include_directories(ast PUBLIC .)
target_link_libraries(ast PUBLIC smbase)

add_executable(
    astgen
    agrampar.cc
    # ast.ast.cc
    ast.ast.h
    ast.hand.cc
    ast.hand.h
    astgen.cc
    ${BISON_agrampar_OUTPUTS}
    ${FLEX_agramlex_OUTPUTS})
target_include_directories(astgen PUBLIC ${CMAKE_CURRENT_BINARY_DIR} .)
target_link_libraries(astgen PRIVATE ast)

install(TARGETS astgen RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if(FALSE)
    # TODO refactor in progress, doesn't build
    set(EXAMPLE_CC ${CMAKE_CURRENT_BINARY_DIR}/example.cc)
    set(EXAMPLE_H ${CMAKE_CURRENT_BINARY_DIR}/example.h)
    add_astgen_command(
        INPUTS
        ${CMAKE_CURRENT_SOURCE_DIR}/example.ast
        OUTPUT_PREFIX
        ${CMAKE_CURRENT_BINARY_DIR}/example
        OUTPUTS
        ${EXAMPLE_CC}
        ${EXAMPLE_H})

    add_executable(
        exampletest
        exampletest.cc
        asthelp.cc
        asthelp.h
        locstr.h
        locstr.cc
        ${EXAMPLE_CC})
    target_include_directories(exampletest PUBLIC ${CMAKE_CURRENT_BINARY_DIR} .)
    target_link_libraries(exampletest PUBLIC smbase)
endif()
