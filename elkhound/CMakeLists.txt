# Copyright 2022, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

if(NOT MSVC)
    add_compile_options(-fno-strict-aliasing)
endif()
add_compile_definitions(USE_ENDSOURCELOC=1)
add_library(elkhound-grammar OBJECT asockind.cc grammar.cc)
target_link_libraries(elkhound-grammar PUBLIC ast)
target_include_directories(elkhound-grammar PUBLIC .)

flex_target(gramlex gramlex.lex ${CMAKE_CURRENT_BINARY_DIR}/gramlex.yy.cc)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gramast.ast.gen.h
           ${CMAKE_CURRENT_BINARY_DIR}/gramast.ast.gen.cc
    COMMAND
        ${CMAKE_COMMAND} -E rm -f ${CMAKE_CURRENT_BINARY_DIR}/gramast.ast.gen.h
        ${CMAKE_CURRENT_BINARY_DIR}/gramast.ast.gen.cc
    COMMAND
        astgen -o${CMAKE_CURRENT_BINARY_DIR}/gramast.ast.gen ${CMAKE_CURRENT_SOURCE_DIR}/gramast.ast
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gramast.ast
    VERBATIM)
add_library(
    elkhound-grampar OBJECT
    emitcode.cc
    mlsstr.cc
    genml.cc
    grampar.cc
    # gramast.gen.o
    ${CMAKE_CURRENT_BINARY_DIR}/gramast.ast.gen.h
    ${CMAKE_CURRENT_BINARY_DIR}/gramast.ast.gen.cc
    # gramlex.yy.cc
    ${FLEX_gramlex_OUTPUTS})
target_link_libraries(elkhound-grampar PUBLIC smbase ast)
target_include_directories(elkhound-grampar PUBLIC .)

add_library(elkhound-glr OBJECT cyctimer.cc glr.cc parsetables.cc useract.cc)
target_link_libraries(elkhound-glr PUBLIC smbase)

add_library(elkhound-util OBJECT ptreenode.cc ptreeact.cc)
target_link_libraries(elkhound-util PUBLIC smbase)

add_library(
    elkhound-support OBJECT
    c/cc_lang.cc
    c/parssppt.cc
    c/lexer1.cc
    c/lexer1yy.cc
    c/lexer2.cc)
target_link_libraries(elkhound-support PUBLIC smbase ast)
target_include_directories(elkhound-support PUBLIC .)

add_library(elkhound-lib $<TARGET_OBJECTS:elkhound-glr> $<TARGET_OBJECTS:elkhound-util>)
target_include_directories(elkhound-lib PUBLIC .)
target_link_libraries(elkhound-lib PUBLIC smbase ast)

add_executable(
    elkhound
    gramanl.cc
    gramexpl.cc
    $<TARGET_OBJECTS:elkhound-grammar>
    $<TARGET_OBJECTS:elkhound-grampar>
    grampar.tab.cc
    parsetables.cc)
target_compile_definitions(elkhound PRIVATE GRAMANL_MAIN)
target_link_libraries(elkhound PUBLIC smbase ast)

if(FALSE)
    # can't find c.tok
    file(READ cc2/cc2.gr ORIG_GR)
    string(REGEX REPLACE "cc2[.]gr[.]gen[.]h" "cc2t/gr.gen.h" CC2_GR ${ORIG_GR})
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/cc2/cc2t.gr ${CC2_GR})

    set(TRGRAMANL ,lrtable)
    add_custom_command(
        OUTPUT
            ${CMAKE_CURRENT_BINARY_DIR}/cc2/cc2t.gr.gen.cc
            ${CMAKE_CURRENT_BINARY_DIR}/cc2/cc2t.gr.gen.h
            ${CMAKE_CURRENT_BINARY_DIR}/cc2/cc2t.gr.gen.y
        COMMAND ${CMAKE_COMMAND} -E rm -f ${CMAKE_CURRENT_BINARY_DIR}/cc2/cc2t.gr.*
        COMMAND
            elkhound -v -tr treebuild${TRGRAMANL} -o ${CMAKE_CURRENT_BINARY_DIR}/cc2/cc2t.gr.gen
            cc2/cc2.gr
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/cc2/cc2t.gr)

    add_executable(cc2t cc2/cc2main.cc ${CMAKE_CURRENT_BINARY_DIR}/cc2/cc2t.gr.gen.cc
                        ${CMAKE_CURRENT_BINARY_DIR}/cc2/cc2t.gr.gen.h)
    target_link_libraries(cc2t PUBLIC elkhound-lib)
endif()
