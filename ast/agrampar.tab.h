/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOK_NAME = 258,
     TOK_INTLIT = 259,
     TOK_EMBEDDED_CODE = 260,
     TOK_LBRACE = 261,
     TOK_RBRACE = 262,
     TOK_SEMICOLON = 263,
     TOK_ARROW = 264,
     TOK_LPAREN = 265,
     TOK_RPAREN = 266,
     TOK_LANGLE = 267,
     TOK_RANGLE = 268,
     TOK_STAR = 269,
     TOK_AMPERSAND = 270,
     TOK_COMMA = 271,
     TOK_EQUALS = 272,
     TOK_COLON = 273,
     TOK_CLASS = 274,
     TOK_PUBLIC = 275,
     TOK_PRIVATE = 276,
     TOK_PROTECTED = 277,
     TOK_VERBATIM = 278,
     TOK_IMPL_VERBATIM = 279,
     TOK_XML_VERBATIM = 280,
     TOK_CTOR = 281,
     TOK_DTOR = 282,
     TOK_PURE_VIRTUAL = 283,
     TOK_CUSTOM = 284,
     TOK_OPTION = 285,
     TOK_NEW = 286,
     TOK_ENUM = 287
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 2068 of yacc.c  */
#line 69 "agrampar.y"

  ASTSpecFile *file;
  ASTList<ToplevelForm> *formList;
  TF_class *tfClass;
  ASTList<CtorArg> *ctorArgList;
  ASTList<Annotation> *userDeclList;
  string *str;
  enum AccessCtl accessCtl;
  AccessMod *accessMod;
  ToplevelForm *verbatim;
  Annotation *annotation;
  TF_option *tfOption;
  ASTList<string> *stringList;
  TF_enum *tfEnum;
  ASTList<string> *enumeratorList;
  string *enumerator;
  ASTList<BaseClass> *baseClassList;
  BaseClass *baseClass;
  CustomCode *customCode;



/* Line 2068 of yacc.c  */
#line 105 "agrampar.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif




