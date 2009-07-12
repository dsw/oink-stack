/* A Bison parser, made by GNU Bison 2.1.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

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
/* Tokens.  */
#define TOK_NAME 258
#define TOK_INTLIT 259
#define TOK_EMBEDDED_CODE 260
#define TOK_LBRACE 261
#define TOK_RBRACE 262
#define TOK_SEMICOLON 263
#define TOK_ARROW 264
#define TOK_LPAREN 265
#define TOK_RPAREN 266
#define TOK_LANGLE 267
#define TOK_RANGLE 268
#define TOK_STAR 269
#define TOK_AMPERSAND 270
#define TOK_COMMA 271
#define TOK_EQUALS 272
#define TOK_COLON 273
#define TOK_CLASS 274
#define TOK_PUBLIC 275
#define TOK_PRIVATE 276
#define TOK_PROTECTED 277
#define TOK_VERBATIM 278
#define TOK_IMPL_VERBATIM 279
#define TOK_XML_VERBATIM 280
#define TOK_CTOR 281
#define TOK_DTOR 282
#define TOK_PURE_VIRTUAL 283
#define TOK_CUSTOM 284
#define TOK_OPTION 285
#define TOK_NEW 286
#define TOK_ENUM 287




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 69 "agrampar.y"
typedef union YYSTYPE {
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
} YYSTYPE;
/* Line 1447 of yacc.c.  */
#line 123 "agrampar.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif





