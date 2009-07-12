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

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 0



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




/* Copy the first part of user declarations.  */
#line 6 "agrampar.y"


#include "agrampar.h"       // agrampar_yylex, etc.

#include <stdlib.h>         // malloc, free
#include <iostream.h>       // cout

// enable debugging the parser
#ifndef NDEBUG
  #define YYDEBUG 1
#endif

// permit having other parser's codes in the same program
#define yyparse agrampar_yyparse



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

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
/* Line 196 of yacc.c.  */
#line 187 "agrampar.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 219 of yacc.c.  */
#line 199 "agrampar.tab.c"

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T) && (defined (__STDC__) || defined (__cplusplus))
# include <stddef.h> /* INFRINGES ON USER NAME SPACE */
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if defined (__STDC__) || defined (__cplusplus)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     define YYINCLUDED_STDLIB_H
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2005 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM ((YYSIZE_T) -1)
#  endif
#  ifdef __cplusplus
extern "C" {
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if (! defined (malloc) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if (! defined (free) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifdef __cplusplus
}
#  endif
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE))			\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   114

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  33
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  30
/* YYNRULES -- Number of rules. */
#define YYNRULES  73
/* YYNRULES -- Number of states. */
#define YYNSTATES  116

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   287

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    12,    15,    18,    21,
      24,    31,    39,    40,    42,    46,    48,    49,    56,    65,
      68,    69,    71,    74,    78,    80,    84,    86,    89,    91,
      93,    97,    99,   101,   103,   105,   107,   111,   112,   115,
     118,   124,   126,   130,   133,   137,   139,   141,   143,   145,
     147,   149,   151,   156,   158,   162,   165,   168,   171,   176,
     177,   180,   186,   193,   195,   199,   201,   202,   205,   207,
     211,   213,   215,   217
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      34,     0,    -1,    35,    -1,    -1,    35,    36,    -1,    35,
      53,    -1,    35,    54,    -1,    35,    56,    -1,    35,    48,
      -1,    35,     8,    -1,    37,    19,     3,    40,    59,    38,
      -1,    37,    19,     3,    41,    41,    59,    38,    -1,    -1,
      31,    -1,     6,    39,     7,    -1,     8,    -1,    -1,    39,
       9,     3,    40,    59,     8,    -1,    39,     9,     3,    40,
      59,     6,    46,     7,    -1,    39,    47,    -1,    -1,    41,
      -1,    10,    11,    -1,    10,    42,    11,    -1,    43,    -1,
      42,    16,    43,    -1,    44,    -1,    43,    44,    -1,     3,
      -1,     4,    -1,    12,    45,    13,    -1,    14,    -1,    15,
      -1,    17,    -1,    19,    -1,    43,    -1,    43,    16,    45,
      -1,    -1,    46,    47,    -1,    51,    49,    -1,    51,     5,
      17,     5,     8,    -1,    48,    -1,    29,     3,    49,    -1,
       5,     8,    -1,     6,     5,     7,    -1,    20,    -1,    21,
      -1,    22,    -1,    26,    -1,    27,    -1,    28,    -1,    50,
      -1,    50,    10,    52,    11,    -1,     3,    -1,    52,    16,
       3,    -1,    23,    49,    -1,    24,    49,    -1,    25,    49,
      -1,    30,     3,    55,     8,    -1,    -1,    55,     3,    -1,
      32,     3,     6,    57,     7,    -1,    32,     3,     6,    57,
      16,     7,    -1,    58,    -1,    57,    16,    58,    -1,     3,
      -1,    -1,    18,    60,    -1,    62,    -1,    60,    16,    62,
      -1,    20,    -1,    21,    -1,    22,    -1,    61,     3,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   116,   116,   122,   123,   124,   125,   126,   127,   128,
     133,   137,   145,   146,   158,   160,   168,   169,   171,   173,
     181,   182,   188,   190,   195,   202,   207,   209,   215,   216,
     217,   218,   219,   220,   221,   225,   227,   234,   235,   241,
     243,   245,   251,   257,   259,   265,   266,   267,   268,   269,
     270,   274,   276,   281,   283,   288,   290,   292,   297,   303,
     304,   309,   311,   316,   318,   323,   329,   330,   335,   337,
     343,   344,   345,   349
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TOK_NAME", "TOK_INTLIT",
  "TOK_EMBEDDED_CODE", "\"{\"", "\"}\"", "\";\"", "\"->\"", "\"(\"",
  "\")\"", "\"<\"", "\">\"", "\"*\"", "\"&\"", "\",\"", "\"=\"", "\":\"",
  "\"class\"", "\"public\"", "\"private\"", "\"protected\"",
  "\"verbatim\"", "\"impl_verbatim\"", "\"xml_verbatim\"", "\"ctor\"",
  "\"dtor\"", "\"pure_virtual\"", "\"custom\"", "\"option\"", "\"new\"",
  "\"enum\"", "$accept", "StartSymbol", "Input", "Class", "NewOpt",
  "ClassBody", "ClassMembersOpt", "CtorArgsOpt", "CtorArgs", "CtorArgList",
  "Arg", "ArgWord", "ArgList", "CtorMembersOpt", "Annotation",
  "CustomCode", "Embedded", "Public", "AccessMod", "StringList",
  "Verbatim", "Option", "OptionArgs", "Enum", "EnumeratorSeq",
  "Enumerator", "BaseClassesOpt", "BaseClassSeq", "BaseAccess",
  "BaseClass", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    33,    34,    35,    35,    35,    35,    35,    35,    35,
      36,    36,    37,    37,    38,    38,    39,    39,    39,    39,
      40,    40,    41,    41,    42,    42,    43,    43,    44,    44,
      44,    44,    44,    44,    44,    45,    45,    46,    46,    47,
      47,    47,    48,    49,    49,    50,    50,    50,    50,    50,
      50,    51,    51,    52,    52,    53,    53,    53,    54,    55,
      55,    56,    56,    57,    57,    58,    59,    59,    60,    60,
      61,    61,    61,    62
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     0,     2,     2,     2,     2,     2,     2,
       6,     7,     0,     1,     3,     1,     0,     6,     8,     2,
       0,     1,     2,     3,     1,     3,     1,     2,     1,     1,
       3,     1,     1,     1,     1,     1,     3,     0,     2,     2,
       5,     1,     3,     2,     3,     1,     1,     1,     1,     1,
       1,     1,     4,     1,     3,     2,     2,     2,     4,     0,
       2,     5,     6,     1,     3,     1,     0,     2,     1,     3,
       1,     1,     1,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       3,     0,     2,     1,     9,     0,     0,     0,     0,     0,
      13,     0,     4,     0,     8,     5,     6,     7,     0,     0,
      55,    56,    57,     0,    59,     0,     0,    43,     0,    42,
       0,     0,    20,    44,    60,    58,    65,     0,    63,     0,
      66,    21,    61,     0,    28,    29,    22,     0,    31,    32,
      33,    34,     0,    24,    26,     0,     0,    66,    62,    64,
      35,     0,    23,     0,    27,    70,    71,    72,    67,     0,
      68,    16,    15,    10,     0,     0,    30,    25,     0,    73,
       0,    11,    36,    69,    14,     0,    45,    46,    47,    48,
      49,    50,    19,    41,    51,     0,    20,     0,     0,    39,
      66,    21,    53,     0,     0,     0,    52,     0,     0,    37,
      17,    54,    40,     0,    18,    38
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,     1,     2,    12,    13,    73,    80,    40,    41,    52,
      60,    54,    61,   113,    92,    93,    20,    94,    95,   103,
      15,    16,    30,    17,    37,    38,    56,    68,    69,    70
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -55
static const yysigned_char yypact[] =
{
     -55,    11,    -4,   -55,   -55,    26,    26,    26,    20,    36,
     -55,    70,   -55,    33,   -55,   -55,   -55,   -55,    52,    72,
     -55,   -55,   -55,    26,   -55,    69,    79,   -55,    83,   -55,
       5,    88,    76,   -55,   -55,   -55,   -55,     0,   -55,    47,
      74,    76,   -55,     7,   -55,   -55,   -55,    81,   -55,   -55,
     -55,   -55,    22,    81,   -55,    49,    57,    74,   -55,   -55,
      64,    84,   -55,    81,   -55,   -55,   -55,   -55,    78,    96,
     -55,   -55,   -55,   -55,    57,    81,   -55,    81,    49,   -55,
      15,   -55,   -55,   -55,   -55,    98,   -55,   -55,   -55,   -55,
     -55,   -55,   -55,   -55,    92,    82,    76,   100,     1,   -55,
      74,   -55,   -55,    29,    99,    66,   -55,   102,   101,   -55,
     -55,   -55,   -55,    27,   -55,   -55
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -55,   -55,   -55,   -55,   -55,    32,   -55,    12,   -39,   -55,
     -33,   -48,    35,   -55,    -2,   105,    -6,   -55,   -55,   -55,
     -55,   -55,   -55,   -55,   -55,    71,   -54,   -55,   -55,    34
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -13
static const yysigned_char yytable[] =
{
      21,    22,    57,    74,     4,    64,    53,    42,    34,    27,
      36,     3,    64,    35,    58,   -12,    43,    29,   104,     5,
       6,     7,    84,    23,    85,     8,     9,    10,    11,    64,
      77,    18,    19,    62,   114,    86,    87,    88,    63,    24,
     106,    89,    90,    91,     8,   107,   105,    86,    87,    88,
      44,    45,    26,    89,    90,    91,     8,   101,    46,    47,
      27,    48,    49,    71,    50,    72,    51,    44,    45,    65,
      66,    67,   109,    25,   110,    31,    47,    28,    48,    49,
      75,    50,    32,    51,    44,    45,    39,    98,    19,    99,
      33,    36,    55,    47,    78,    48,    49,    76,    50,    79,
      51,    96,    97,   102,   108,   111,    81,    14,   100,   112,
      82,   115,    83,     0,    59
};

static const yysigned_char yycheck[] =
{
       6,     7,    41,    57,     8,    53,    39,     7,     3,     8,
       3,     0,    60,     8,     7,    19,    16,    23,    17,    23,
      24,    25,     7,     3,     9,    29,    30,    31,    32,    77,
      63,     5,     6,    11,     7,    20,    21,    22,    16,     3,
      11,    26,    27,    28,    29,    16,   100,    20,    21,    22,
       3,     4,    19,    26,    27,    28,    29,    96,    11,    12,
       8,    14,    15,     6,    17,     8,    19,     3,     4,    20,
      21,    22,     6,     3,     8,     6,    12,     5,    14,    15,
      16,    17,     3,    19,     3,     4,    10,     5,     6,    95,
       7,     3,    18,    12,    16,    14,    15,    13,    17,     3,
      19,     3,    10,     3,     5,     3,    74,     2,    96,     8,
      75,   113,    78,    -1,    43
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    34,    35,     0,     8,    23,    24,    25,    29,    30,
      31,    32,    36,    37,    48,    53,    54,    56,     5,     6,
      49,    49,    49,     3,     3,     3,    19,     8,     5,    49,
      55,     6,     3,     7,     3,     8,     3,    57,    58,    10,
      40,    41,     7,    16,     3,     4,    11,    12,    14,    15,
      17,    19,    42,    43,    44,    18,    59,    41,     7,    58,
      43,    45,    11,    16,    44,    20,    21,    22,    60,    61,
      62,     6,     8,    38,    59,    16,    13,    43,    16,     3,
      39,    38,    45,    62,     7,     9,    20,    21,    22,    26,
      27,    28,    47,    48,    50,    51,     3,    10,     5,    49,
      40,    41,     3,    52,    17,    59,    11,    16,     5,     6,
       8,     3,     8,    46,     7,    47
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (0)


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (N)								\
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (0)
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
              (Loc).first_line, (Loc).first_column,	\
              (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr,					\
                  Type, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname[yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      size_t yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

#endif /* YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);


# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */






/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()
    ;
#endif
#endif
{
  /* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;

  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  short int *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short int *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short int *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a look-ahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to look-ahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 117 "agrampar.y"
    { (yyval.file) = *((ASTSpecFile**)parseParam) = new ASTSpecFile((yyvsp[0].formList)); ;}
    break;

  case 3:
#line 122 "agrampar.y"
    { (yyval.formList) = new ASTList<ToplevelForm>; ;}
    break;

  case 4:
#line 123 "agrampar.y"
    { ((yyval.formList)=(yyvsp[-1].formList))->append((yyvsp[0].tfClass)); ;}
    break;

  case 5:
#line 124 "agrampar.y"
    { ((yyval.formList)=(yyvsp[-1].formList))->append((yyvsp[0].verbatim)); ;}
    break;

  case 6:
#line 125 "agrampar.y"
    { ((yyval.formList)=(yyvsp[-1].formList))->append((yyvsp[0].tfOption)); ;}
    break;

  case 7:
#line 126 "agrampar.y"
    { ((yyval.formList)=(yyvsp[-1].formList))->append((yyvsp[0].tfEnum)); ;}
    break;

  case 8:
#line 127 "agrampar.y"
    { ((yyval.formList)=(yyvsp[-1].formList))->append(new TF_custom((yyvsp[0].customCode))); ;}
    break;

  case 9:
#line 128 "agrampar.y"
    { (yyval.formList)=(yyvsp[-1].formList); ;}
    break;

  case 10:
#line 134 "agrampar.y"
    { ((yyval.tfClass)=(yyvsp[0].tfClass))->super->name = unbox((yyvsp[-3].str)); 
           (yyval.tfClass)->super->args.steal((yyvsp[-2].ctorArgList)); 
           (yyval.tfClass)->super->bases.steal((yyvsp[-1].baseClassList)); ;}
    break;

  case 11:
#line 138 "agrampar.y"
    { ((yyval.tfClass)=(yyvsp[0].tfClass))->super->name = unbox((yyvsp[-4].str));
           (yyval.tfClass)->super->args.steal((yyvsp[-3].ctorArgList));
           (yyval.tfClass)->super->lastArgs.steal((yyvsp[-2].ctorArgList));
           (yyval.tfClass)->super->bases.steal((yyvsp[-1].baseClassList)); ;}
    break;

  case 12:
#line 145 "agrampar.y"
    {;}
    break;

  case 13:
#line 146 "agrampar.y"
    {;}
    break;

  case 14:
#line 159 "agrampar.y"
    { (yyval.tfClass)=(yyvsp[-1].tfClass); ;}
    break;

  case 15:
#line 161 "agrampar.y"
    { (yyval.tfClass) = new TF_class(new ASTClass("(placeholder)", NULL, NULL, NULL, NULL), NULL); ;}
    break;

  case 16:
#line 168 "agrampar.y"
    { (yyval.tfClass) = new TF_class(new ASTClass("(placeholder)", NULL, NULL, NULL, NULL), NULL); ;}
    break;

  case 17:
#line 170 "agrampar.y"
    { ((yyval.tfClass)=(yyvsp[-5].tfClass))->ctors.append(new ASTClass(unbox((yyvsp[-3].str)), (yyvsp[-2].ctorArgList), NULL, (yyvsp[-1].baseClassList), NULL)); ;}
    break;

  case 18:
#line 172 "agrampar.y"
    { ((yyval.tfClass)=(yyvsp[-7].tfClass))->ctors.append(new ASTClass(unbox((yyvsp[-5].str)), (yyvsp[-4].ctorArgList), NULL, (yyvsp[-3].baseClassList), (yyvsp[-1].userDeclList))); ;}
    break;

  case 19:
#line 174 "agrampar.y"
    { ((yyval.tfClass)=(yyvsp[-1].tfClass))->super->decls.append((yyvsp[0].annotation)); ;}
    break;

  case 20:
#line 181 "agrampar.y"
    { (yyval.ctorArgList) = new ASTList<CtorArg>; ;}
    break;

  case 21:
#line 183 "agrampar.y"
    { (yyval.ctorArgList) = (yyvsp[0].ctorArgList); ;}
    break;

  case 22:
#line 189 "agrampar.y"
    { (yyval.ctorArgList) = new ASTList<CtorArg>; ;}
    break;

  case 23:
#line 191 "agrampar.y"
    { (yyval.ctorArgList) = (yyvsp[-1].ctorArgList); ;}
    break;

  case 24:
#line 196 "agrampar.y"
    { (yyval.ctorArgList) = new ASTList<CtorArg>;
                 {
                   string tmp = unbox((yyvsp[0].str));
                   (yyval.ctorArgList)->append(parseCtorArg(tmp));
                 }
               ;}
    break;

  case 25:
#line 203 "agrampar.y"
    { ((yyval.ctorArgList)=(yyvsp[-2].ctorArgList))->append(parseCtorArg(unbox((yyvsp[0].str)))); ;}
    break;

  case 26:
#line 208 "agrampar.y"
    { (yyval.str) = (yyvsp[0].str); ;}
    break;

  case 27:
#line 210 "agrampar.y"
    { (yyval.str) = appendStr((yyvsp[-1].str), (yyvsp[0].str)); ;}
    break;

  case 28:
#line 215 "agrampar.y"
    { (yyval.str) = appendStr((yyvsp[0].str), box(" ")); ;}
    break;

  case 29:
#line 216 "agrampar.y"
    { (yyval.str) = appendStr((yyvsp[0].str), box(" ")); ;}
    break;

  case 30:
#line 217 "agrampar.y"
    { (yyval.str) = appendStr(box("<"), appendStr((yyvsp[-1].str), box(">"))); ;}
    break;

  case 31:
#line 218 "agrampar.y"
    { (yyval.str) = box("*"); ;}
    break;

  case 32:
#line 219 "agrampar.y"
    { (yyval.str) = box("&"); ;}
    break;

  case 33:
#line 220 "agrampar.y"
    { (yyval.str) = box("="); ;}
    break;

  case 34:
#line 221 "agrampar.y"
    { (yyval.str) = box("class "); ;}
    break;

  case 35:
#line 226 "agrampar.y"
    { (yyval.str) = (yyvsp[0].str); ;}
    break;

  case 36:
#line 228 "agrampar.y"
    { (yyval.str) = appendStr((yyvsp[-2].str), appendStr(box(","), (yyvsp[0].str))); ;}
    break;

  case 37:
#line 234 "agrampar.y"
    { (yyval.userDeclList) = new ASTList<Annotation>; ;}
    break;

  case 38:
#line 236 "agrampar.y"
    { ((yyval.userDeclList)=(yyvsp[-1].userDeclList))->append((yyvsp[0].annotation)); ;}
    break;

  case 39:
#line 242 "agrampar.y"
    { (yyval.annotation) = new UserDecl((yyvsp[-1].accessMod), unbox((yyvsp[0].str)), ""); ;}
    break;

  case 40:
#line 244 "agrampar.y"
    { (yyval.annotation) = new UserDecl((yyvsp[-4].accessMod), unbox((yyvsp[-3].str)), unbox((yyvsp[-1].str))); ;}
    break;

  case 41:
#line 246 "agrampar.y"
    { (yyval.annotation) = (yyvsp[0].customCode); ;}
    break;

  case 42:
#line 252 "agrampar.y"
    { (yyval.customCode) = new CustomCode(unbox((yyvsp[-1].str)), unbox((yyvsp[0].str))); ;}
    break;

  case 43:
#line 258 "agrampar.y"
    { (yyval.str) = (yyvsp[-1].str); ;}
    break;

  case 44:
#line 260 "agrampar.y"
    { (yyval.str) = (yyvsp[-1].str); ;}
    break;

  case 45:
#line 265 "agrampar.y"
    { (yyval.accessCtl) = AC_PUBLIC; ;}
    break;

  case 46:
#line 266 "agrampar.y"
    { (yyval.accessCtl) = AC_PRIVATE; ;}
    break;

  case 47:
#line 267 "agrampar.y"
    { (yyval.accessCtl) = AC_PROTECTED; ;}
    break;

  case 48:
#line 268 "agrampar.y"
    { (yyval.accessCtl) = AC_CTOR; ;}
    break;

  case 49:
#line 269 "agrampar.y"
    { (yyval.accessCtl) = AC_DTOR; ;}
    break;

  case 50:
#line 270 "agrampar.y"
    { (yyval.accessCtl) = AC_PUREVIRT; ;}
    break;

  case 51:
#line 275 "agrampar.y"
    { (yyval.accessMod) = new AccessMod((yyvsp[0].accessCtl), NULL); ;}
    break;

  case 52:
#line 277 "agrampar.y"
    { (yyval.accessMod) = new AccessMod((yyvsp[-3].accessCtl), (yyvsp[-1].stringList)); ;}
    break;

  case 53:
#line 282 "agrampar.y"
    { (yyval.stringList) = new ASTList<string>((yyvsp[0].str)); ;}
    break;

  case 54:
#line 284 "agrampar.y"
    { ((yyval.stringList)=(yyvsp[-2].stringList))->append((yyvsp[0].str)); ;}
    break;

  case 55:
#line 289 "agrampar.y"
    { (yyval.verbatim) = new TF_verbatim(unbox((yyvsp[0].str))); ;}
    break;

  case 56:
#line 291 "agrampar.y"
    { (yyval.verbatim) = new TF_impl_verbatim(unbox((yyvsp[0].str))); ;}
    break;

  case 57:
#line 293 "agrampar.y"
    { (yyval.verbatim) = new TF_xml_verbatim(unbox((yyvsp[0].str))); ;}
    break;

  case 58:
#line 298 "agrampar.y"
    { (yyval.tfOption) = new TF_option(unbox((yyvsp[-2].str)), (yyvsp[-1].stringList)); ;}
    break;

  case 59:
#line 303 "agrampar.y"
    { (yyval.stringList) = new ASTList<string>; ;}
    break;

  case 60:
#line 305 "agrampar.y"
    { ((yyval.stringList)=(yyvsp[-1].stringList))->append((yyvsp[0].str)); ;}
    break;

  case 61:
#line 310 "agrampar.y"
    { (yyval.tfEnum) = new TF_enum(unbox((yyvsp[-3].str)), (yyvsp[-1].enumeratorList)); ;}
    break;

  case 62:
#line 312 "agrampar.y"
    { (yyval.tfEnum) = new TF_enum(unbox((yyvsp[-4].str)), (yyvsp[-2].enumeratorList)); ;}
    break;

  case 63:
#line 317 "agrampar.y"
    { (yyval.enumeratorList) = new ASTList<string>((yyvsp[0].enumerator)); ;}
    break;

  case 64:
#line 319 "agrampar.y"
    { ((yyval.enumeratorList)=(yyvsp[-2].enumeratorList))->append((yyvsp[0].enumerator)); ;}
    break;

  case 65:
#line 324 "agrampar.y"
    { (yyval.enumerator) = (yyvsp[0].str); ;}
    break;

  case 66:
#line 329 "agrampar.y"
    { (yyval.baseClassList) = new ASTList<BaseClass>; ;}
    break;

  case 67:
#line 331 "agrampar.y"
    { (yyval.baseClassList) = (yyvsp[0].baseClassList); ;}
    break;

  case 68:
#line 336 "agrampar.y"
    { (yyval.baseClassList) = new ASTList<BaseClass>((yyvsp[0].baseClass)); ;}
    break;

  case 69:
#line 338 "agrampar.y"
    { ((yyval.baseClassList)=(yyvsp[-2].baseClassList))->append((yyvsp[0].baseClass)); ;}
    break;

  case 70:
#line 343 "agrampar.y"
    { (yyval.accessCtl) = AC_PUBLIC; ;}
    break;

  case 71:
#line 344 "agrampar.y"
    { (yyval.accessCtl) = AC_PRIVATE; ;}
    break;

  case 72:
#line 345 "agrampar.y"
    { (yyval.accessCtl) = AC_PROTECTED; ;}
    break;

  case 73:
#line 350 "agrampar.y"
    { (yyval.baseClass) = new BaseClass((yyvsp[-1].accessCtl), unbox((yyvsp[0].str))); ;}
    break;


      default: break;
    }

/* Line 1126 of yacc.c.  */
#line 1667 "agrampar.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  int yytype = YYTRANSLATE (yychar);
	  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
	  YYSIZE_T yysize = yysize0;
	  YYSIZE_T yysize1;
	  int yysize_overflow = 0;
	  char *yymsg = 0;
#	  define YYERROR_VERBOSE_ARGS_MAXIMUM 5
	  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
	  int yyx;

#if 0
	  /* This is so xgettext sees the translatable formats that are
	     constructed on the fly.  */
	  YY_("syntax error, unexpected %s");
	  YY_("syntax error, unexpected %s, expecting %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
#endif
	  char *yyfmt;
	  char const *yyf;
	  static char const yyunexpected[] = "syntax error, unexpected %s";
	  static char const yyexpecting[] = ", expecting %s";
	  static char const yyor[] = " or %s";
	  char yyformat[sizeof yyunexpected
			+ sizeof yyexpecting - 1
			+ ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
			   * (sizeof yyor - 1))];
	  char const *yyprefix = yyexpecting;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 1;

	  yyarg[0] = yytname[yytype];
	  yyfmt = yystpcpy (yyformat, yyunexpected);

	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
		  {
		    yycount = 1;
		    yysize = yysize0;
		    yyformat[sizeof yyunexpected - 1] = '\0';
		    break;
		  }
		yyarg[yycount++] = yytname[yyx];
		yysize1 = yysize + yytnamerr (0, yytname[yyx]);
		yysize_overflow |= yysize1 < yysize;
		yysize = yysize1;
		yyfmt = yystpcpy (yyfmt, yyprefix);
		yyprefix = yyor;
	      }

	  yyf = YY_(yyformat);
	  yysize1 = yysize + yystrlen (yyf);
	  yysize_overflow |= yysize1 < yysize;
	  yysize = yysize1;

	  if (!yysize_overflow && yysize <= YYSTACK_ALLOC_MAXIMUM)
	    yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg)
	    {
	      /* Avoid sprintf, as that infringes on the user's name space.
		 Don't have undefined behavior even if the translation
		 produced a string with the wrong number of "%s"s.  */
	      char *yyp = yymsg;
	      int yyi = 0;
	      while ((*yyp = *yyf))
		{
		  if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		    {
		      yyp += yytnamerr (yyp, yyarg[yyi++]);
		      yyf += 2;
		    }
		  else
		    {
		      yyp++;
		      yyf++;
		    }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    {
	      yyerror (YY_("syntax error"));
	      goto yyexhaustedlab;
	    }
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror (YY_("syntax error"));
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
        }
      else
	{
	  yydestruct ("Error: discarding", yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (0)
     goto yyerrorlab;

yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping", yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token. */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK;
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 353 "agrampar.y"


/* ----------------- extra C code ------------------- */


