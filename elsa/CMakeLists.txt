# Copyright 2022, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0
find_package(Perl REQUIRED)
set(TOK_MODS ${CMAKE_CURRENT_SOURCE_DIR}/cc_tokens.tok ${CMAKE_CURRENT_SOURCE_DIR}/gnu_ext.tok)
set(LEXER_MODS ${CMAKE_CURRENT_SOURCE_DIR}/cc.lex ${CMAKE_CURRENT_SOURCE_DIR}/gnu.lex)
set(CC_AST_MODS
    ${CMAKE_CURRENT_SOURCE_DIR}/cc.ast
    ${CMAKE_CURRENT_SOURCE_DIR}/cc_tcheck.ast
    ${CMAKE_CURRENT_SOURCE_DIR}/cc_print.ast
    ${CMAKE_CURRENT_SOURCE_DIR}/cfg.ast
    ${CMAKE_CURRENT_SOURCE_DIR}/cc_elaborate.ast
    ${CMAKE_CURRENT_SOURCE_DIR}/bpprint.ast
    ${CMAKE_CURRENT_SOURCE_DIR}/cc2c.ast
    ${CMAKE_CURRENT_SOURCE_DIR}/gnu.ast
    ${CMAKE_CURRENT_SOURCE_DIR}/kandr.ast)
set(CC_GR_MODS ${CMAKE_CURRENT_SOURCE_DIR}/cc.gr ${CMAKE_CURRENT_SOURCE_DIR}/gnu.gr
               ${CMAKE_CURRENT_SOURCE_DIR}/kandr.gr)

# XML
set(XML_TYPE_LEXER
    ${CMAKE_CURRENT_BINARY_DIR}/xml_enum_1.gen.h ${CMAKE_CURRENT_BINARY_DIR}/xml_lex_1.gen.lex
    ${CMAKE_CURRENT_BINARY_DIR}/xml_name_1.gen.cc)
set(XML_TOKENS_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/xml_basic.tokens ${CMAKE_CURRENT_SOURCE_DIR}/xml_file.tokens
    ${CMAKE_CURRENT_SOURCE_DIR}/xml_type.tokens ${CMAKE_CURRENT_SOURCE_DIR}/xml_ast.gen.tokens)
add_custom_command(
    OUTPUT ${XML_TYPE_LEXER}
    DEPENDS ${XML_TOKENS_FILES} token.pl
    COMMAND ${PERL_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/token.pl ${XML_TOKENS_FILES}
    COMMENT "Generating basic, file, and typesystem xml lexer files by token.pl"
    VERBATIM)

set(XML_ASTGEN
    ${CMAKE_CURRENT_BINARY_DIR}/xml_ast.gen.tokens
    ${CMAKE_CURRENT_BINARY_DIR}/xml_ast_reader_0decl.gen.h
    ${CMAKE_CURRENT_BINARY_DIR}/xml_ast_reader_1defn.gen.cc
    ${CMAKE_CURRENT_BINARY_DIR}/xml_ast_reader_2ctrc.gen.cc)

add_custom_command(
    OUTPUT ${XML_ASTGEN}
    # COMMAND ${CMAKE_COMMAND} -E rm -f ${XML_ASTGEN}
    COMMAND astgen -tr no_ast.gen,xmlParser -v -o${CMAKE_CURRENT_BINARY_DIR}/xml_ast ${CC_AST_MODS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${CC_AST_MODS}
    USES_TERMINAL VERBATIM)

add_custom_target(elsa_xml_generated_files DEPENDS ${XML_TYPE_LEXER} ${XML_ASTGEN})

set(LEX_INPUT
    ${CMAKE_CURRENT_SOURCE_DIR}/xml_lex_0top.lex ${CMAKE_CURRENT_BINARY_DIR}/xml_lex_1.gen.lex
    ${CMAKE_CURRENT_SOURCE_DIR}/xml_lex_2bot.lex)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/xml_lex.gen.lex
    COMMAND ${CMAKE_COMMAND} -E cat ${LEX_INPUT} > ${CMAKE_CURRENT_BINARY_DIR}/xml_lex.gen.lex
    DEPENDS ${LEX_INPUT}
    VERBATIM)

flex_target(xml_lex ${CMAKE_CURRENT_BINARY_DIR}/xml_lex.gen.lex
            ${CMAKE_CURRENT_BINARY_DIR}/xml_lex.gen.yy.cc)

add_library(
    elsa_xml_objs OBJECT
    xml_lexer.cc
    xml_reader.cc
    xml_writer.cc
    xml_file_reader.cc
    xml_file_writer.cc
    xml_type_reader.cc
    xml_type_writer.cc
    xml_ast_reader.cc
    id_obj_dict.cc
    xml_do_read.cc
    # ${XML_ASTGEN}
    # ${XML_TYPE_LEXER}
    ${FLEX_xml_lex_OUTPUTS})
target_link_libraries(elsa_xml_objs PUBLIC smbase elkhound-lib ast)
target_include_directories(elsa_xml_objs PUBLIC .)
# the cc files in here are included from within another cc, so hard to make a clean dep
add_dependencies(elsa_xml_objs elsa_xml_generated_files)

set(TOK_FILES ${CMAKE_CURRENT_BINARY_DIR}/cc_tokens.h ${CMAKE_CURRENT_BINARY_DIR}/cc_tokens.cc
              ${CMAKE_CURRENT_BINARY_DIR}/cc_tokens.ids)
add_custom_command(
    OUTPUT ${TOK_FILES}
    COMMAND ${CMAKE_COMMAND} -E rm -f ${TOK_FILES}
    COMMAND ${PERL_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/make-token-files ${TOK_MODS}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/make-token-files ${TOK_MODS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lexer.lex
    COMMAND
        ${PERL_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/merge-lexer-exts.pl ${LEXER_MODS} >
        ${CMAKE_CURRENT_BINARY_DIR}/lexer.lex
    DEPENDS ${LEXER_MODS} merge-lexer-exts.pl)
flex_target(lexer ${CMAKE_CURRENT_BINARY_DIR}/lexer.lex ${CMAKE_CURRENT_BINARY_DIR}/lexer.yy.cc)

set(CC_AST_CC ${CMAKE_CURRENT_BINARY_DIR}/cc.ast.gen.cc)
set(CC_AST_H ${CMAKE_CURRENT_BINARY_DIR}/cc.ast.gen.h)

add_custom_command(
    OUTPUT ${CC_AST_CC} ${CC_AST_H}
    COMMAND astgen -v -o${CMAKE_CURRENT_BINARY_DIR}/cc.ast.gen ${CC_AST_MODS}
    DEPENDS ${CC_AST_MODS}
    USES_TERMINAL VERBATIM)

set(CC_GR_MODS cc.gr gnu.gr kandr.gr)

set(GR_GEN_OUTPUTS ${CMAKE_CURRENT_BINARY_DIR}/cc.gr.gen.cc ${CMAKE_CURRENT_BINARY_DIR}/cc.gr.gen.h
                   ${CMAKE_CURRENT_BINARY_DIR}/cc.gr.gen.y)
add_custom_command(
    OUTPUT ${GR_GEN_OUTPUTS} ${CMAKE_CURRENT_BINARY_DIR}/cc.gr.gen.out
    COMMAND ${CMAKE_COMMAND} -E rm -f ${GR_GEN_OUTPUTS}
    COMMAND elkhound -v -tr lrtable -o ${CMAKE_CURRENT_BINARY_DIR}/cc.gr.gen ${CC_GR_MODS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${CC_GR_MODS} cc_tokens.ids)
add_executable(
    ccparse
    bpprint.cc
    ia64mangle.cc
    mtype.cc
    integrity.cc
    astvisit.cc
    template.cc
    notopt.cc
    cc_env.cc
    cc_tcheck.cc
    const_eval.cc
    implint.cc
    serialno.cc
    cc_scope.cc
    cc_elaborate.cc
    ast_build.cc
    $<TARGET_OBJECTS:elsa_xml_objs>
    #   $(LEXER_OBJS)
    #   $(XML_OBJS)
    #   $(EXT_OBJS)
    builtinops.cc
    cfg.cc
    sprint.cc
    mangle.cc
    cc_err.cc
    cc_type.cc
    stdconv.cc
    implconv.cc
    overload.cc
    typelistiter.cc
    ${CC_AST_CC}
    ${CC_AST_H}
    # cc.ast.gen.cc
    # cc.gr.gen.cc
    ${GR_GEN_OUTPUTS}
    parssppt.cc
    cc_flags.cc
    cc_print.cc
    cc_ast_aux.cc
    variable.cc
    lookupset.cc
    ccparse.cc
    cc2c.cc)
target_link_libraries(ccparse PRIVATE smbase ast elkhound-lib)
target_include_directories(ccparse PRIVATE .)
