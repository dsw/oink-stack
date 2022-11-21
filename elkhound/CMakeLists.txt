# Copyright 2022, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

if(NOT MSVC)
    add_compile_options(-fno-strict-aliasing)
endif()

# Configure header
set(GLR_SOURCELOC 1)
set(eef 0)
set(gcs 0)
set(gcsc 0)
set(crs 0)
configure_file(glrconfig.h.in ${CMAKE_CURRENT_BINARY_DIR}/glrconfig.h @ONLY)

add_compile_definitions(USE_ENDSOURCELOC=1)
add_library(elkhound-grammar OBJECT asockind.cc grammar.cc)
target_link_libraries(elkhound-grammar PUBLIC ast)
target_include_directories(elkhound-grammar PUBLIC .)

flex_target(gramlex gramlex.lex ${CMAKE_CURRENT_BINARY_DIR}/gramlex.yy.cc)

bison_target(grampar grampar.y ${CMAKE_CURRENT_BINARY_DIR}/grampar.tab.cc
             DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/grampar.tab.h)
add_flex_bison_dependency(gramlex grampar)

add_astgen_command(
    INPUTS
    ${CMAKE_CURRENT_SOURCE_DIR}/gramast.ast
    OUTPUTS
    ${CMAKE_CURRENT_BINARY_DIR}/gramast.ast.gen.h
    ${CMAKE_CURRENT_BINARY_DIR}/gramast.ast.gen.cc
    OUTPUT_PREFIX
    ${CMAKE_CURRENT_BINARY_DIR}/gramast.ast.gen)

add_library(
    elkhound-grampar OBJECT
    emitcode.cc
    mlsstr.cc
    genml.cc
    grampar.cc
    ${CMAKE_CURRENT_BINARY_DIR}/gramast.ast.gen.h
    ${CMAKE_CURRENT_BINARY_DIR}/gramast.ast.gen.cc
    ${FLEX_gramlex_OUTPUTS})
target_link_libraries(elkhound-grampar PUBLIC smbase ast)
target_include_directories(elkhound-grampar PUBLIC . ${CMAKE_CURRENT_BINARY_DIR})

add_library(
    elkhound-support OBJECT
    c/cc_lang.cc
    c/parssppt.cc
    c/lexer1.cc
    c/lexer1yy.cc
    c/lexer2.cc)
target_link_libraries(elkhound-support PUBLIC smbase ast)
target_include_directories(elkhound-support PUBLIC . ${CMAKE_CURRENT_BINARY_DIR})

add_library(
    elkhound-lib
    # glr-set
    cyctimer.cc
    glr.cc
    parsetables.cc
    useract.cc
    # util-set
    ptreenode.cc
    ptreeact.cc)
target_include_directories(elkhound-lib PUBLIC . ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(elkhound-lib PUBLIC smbase ast)


add_executable(
    elkhound
    gramanl.cc
    gramexpl.cc
    $<TARGET_OBJECTS:elkhound-grammar>
    $<TARGET_OBJECTS:elkhound-grampar>
    ${BISON_grampar_OUTPUTS}
    parsetables.cc)
target_compile_definitions(elkhound PRIVATE GRAMANL_MAIN)
target_link_libraries(elkhound PUBLIC smbase ast)
target_include_directories(elkhound PUBLIC . ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS elkhound RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if(FALSE)
    # needs c.tok from nested c directory
    file(READ cc2/cc2.gr ORIG_GR)
    string(REGEX REPLACE "cc2[.]gr[.]gen[.]h" "cc2t/gr.gen.h" CC2_GR ${ORIG_GR})
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/cc2/cc2t.gr ${CC2_GR})

    add_elkhound_command(
        INPUTS
        cc2/cc2.gr
        OUTPUT_PREFIX
        ${CMAKE_CURRENT_BINARY_DIR}/cc2/cc2t.gr.gen
        CC_OUTPUT_VAR
        CC2T_GEN_OUTPUTS
        WORKING_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}
        TRANSLATE_OPTIONS
        treebuild
        lrtable)

    add_executable(cc2t cc2/cc2main.cc ${CC2T_GEN_OUTPUTS})
    target_link_libraries(cc2t PUBLIC elkhound-lib)
endif()
