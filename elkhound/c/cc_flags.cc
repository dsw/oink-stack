// cc_flags.cc            see license.txt for copyright and terms of use
// code for cc_flags.h

#include "cc_flags.h"     // this module
#include "macros.h"       // STATIC_ASSERT
#include "xassert.h"      // xassert
#include "trace.h"        // tracingSys


// -------------------- TypeIntr -------------------------
char const * const typeIntrNames[NUM_TYPEINTRS] = {
  "struct",
  "class",
  "union",
  "enum"
};

#define MAKE_TOSTRING(T, limit, array)        \
  string toString(T index)                    \
  {                                           \
    xassert((unsigned)index < limit);         \
    return string(array[index]);              \
  }

MAKE_TOSTRING(TypeIntr, NUM_TYPEINTRS, typeIntrNames)


// ---------------- CVFlags -------------
#ifdef MLVALUE
MAKE_ML_TAG(attribute, 0, AId)
MAKE_ML_TAG(attribute, 1, ACons)

MLValue cvToMLAttrs(CVFlags cv)
{
  // AId of string

  MLValue list = mlNil();
  if (cv & CV_CONST) {
    list = mlCons(mlTuple1(attribute_AId, mlString("const")), list);
  }
  if (cv & CV_VOLATILE) {
    list = mlCons(mlTuple1(attribute_AId, mlString("volatile")), list);
  }
  if (cv & CV_OWNER) {
    list = mlCons(mlTuple1(attribute_AId, mlString("owner")), list);
  }
  return list;
}
#endif // MLVALUE


char const * const cvFlagNames[NUM_CVFLAGS] = {
  "const",
  "volatile",
  "owner"
};


string bitmapString(int bitmap, char const * const *names, int numflags)
{
  stringBuilder sb;
  int count=0;
  for (int i=0; i<numflags; i++) {
    if (bitmap & (1 << i)) {
      if (count++) {
        sb << " ";
      }
      sb << names[i];
    }
  }

  return sb;
}

string toString(CVFlags cv)
{
  return bitmapString(cv >> CV_SHIFT_AMOUNT, cvFlagNames, NUM_CVFLAGS);
}


// ------------------- DeclFlags --------------
char const * const declFlagNames[NUM_DECLFLAGS] = {
  "auto",           // 0
  "register",
  "static",
  "extern",
  "mutable",        // 4
  "inline",
  "virtual",
  "explicit",
  "friend",
  "typedef",        // 9

  "<enumerator>",
  "<global>",
  "<initialized>",
  "<builtin>",
  "<logic>",        // 14
  "<addrtaken>",
  "<parameter>",
  "<universal>",
  "<existential>",
  "<member>",       // 19
  "<definition>",
  "<inline_defn>",
  "<implicit>",
  "<forward>",

  "<predicate>",    // 24
};


string toString(DeclFlags df)
{ 
  // make sure I haven't added a flag without adding a string for it
  xassert(declFlagNames[NUM_DECLFLAGS-1] != NULL);

  return bitmapString(df, declFlagNames, NUM_DECLFLAGS);
}


// ---------------------- SimpleTypeId --------------------------
bool isValid(SimpleTypeId id)
{
  return 0 <= id && id <= NUM_SIMPLE_TYPES;
}


static SimpleTypeInfo const simpleTypeInfoArray[] = {
  //name                   size  int?
  { "char",                1,    true    },
  { "unsigned char",       1,    true    },
  { "signed char",         1,    true    },
  { "bool",                4,    true    },
  { "int",                 4,    true    },
  { "unsigned int",        4,    true    },
  { "long int",            4,    true    },
  { "unsigned long int",   4,    true    },
  { "long long",           8,    true    },
  { "unsigned long long",  8,    true    },
  { "short int",           2,    true    },
  { "unsigned short int",  2,    true    },
  { "wchar_t",             2,    true    },
  { "float",               4,    false   },
  { "double",              8,    false   },
  { "long double",         10,   false   },
  { "void",                1,    false   },    // gnu: sizeof(void) is 1
  { "...",                 0,    false   },
  { "/*cdtor*/",           0,    false   },    // dsw: don't want to print <cdtor>
  { "<error>",             0,    false   },
  { "<dependent>",         0,    false   },
};

SimpleTypeInfo const &simpleTypeInfo(SimpleTypeId id)
{
  STATIC_ASSERT(TABLESIZE(simpleTypeInfoArray) == NUM_SIMPLE_TYPES);
  xassert(isValid(id));
  return simpleTypeInfoArray[id];
}


// ------------------------ UnaryOp -----------------------------
char const * const unaryOpNames[NUM_UNARYOPS] = {
  "+",
  "-",
  "!",
  "~"
};

MAKE_TOSTRING(UnaryOp, NUM_UNARYOPS, unaryOpNames)


char const * const effectOpNames[NUM_EFFECTOPS] = {
  "++/*postfix*/",
  "--/*postfix*/",
  "++/*prefix*/",
  "--/*prefix*/",
};

MAKE_TOSTRING(EffectOp, NUM_EFFECTOPS, effectOpNames)

bool isPostfix(EffectOp op)
{
  return op <= EFF_POSTDEC;
}


// ---------------------- BinaryOp -------------------------
char const * const binaryOpNames[NUM_BINARYOPS] = {
  "==",
  "!=",
  "<",
  ">",
  "<=",
  ">=",

  "*",
  "/",
  "%",
  "+",
  "-",
  "<<",
  ">>",
  "&",
  "^",
  "|",
  "&&",
  "||",

  "=",

  ".*",
  "->*",

  "==>"
};

MAKE_TOSTRING(BinaryOp, NUM_BINARYOPS, binaryOpNames)

bool isPredicateCombinator(BinaryOp op)
{
  return op==BIN_AND || op==BIN_OR || op==BIN_IMPLIES;
}

bool isRelational(BinaryOp op)
{
  return BIN_EQUAL <= op && op <= BIN_GREATEREQ;
}


// ------------------- AccessKeyword -------------------
char const * const accessKeywordNames[NUM_ACCESS_KEYWORDS] = {
  "public",
  "protected",
  "private",
  "unspecified"
};

MAKE_TOSTRING(AccessKeyword, NUM_ACCESS_KEYWORDS, accessKeywordNames)


// -------------------- CastKeyword --------------------
char const * const castKeywordNames[NUM_CAST_KEYWORDS] = {
  "dynamic_cast",
  "static_cast",
  "reinterpret_cast",
  "const_cast"
};

MAKE_TOSTRING(CastKeyword, NUM_CAST_KEYWORDS, castKeywordNames)


// -------------------- OverloadableOp --------------------
char const * const overloadableOpNames[NUM_OVERLOADABLE_OPS] = {
  ",",
  "->",
  "()",
  "[]"
};

MAKE_TOSTRING(OverloadableOp, NUM_OVERLOADABLE_OPS, overloadableOpNames)


// ------------------------ UberModifiers ---------------------
char const * const uberModifierNames[UM_NUM_FLAGS] = {
  "auto",
  "register",
  "static",
  "extern",
  "mutable",
  "inline",
  "virtual",
  "explicit",
  "friend",
  "typedef",

  "const",
  "volatile",
  
  "char",
  "wchar_t",
  "bool",
  "short",
  "int",
  "long",
  "signed",
  "unsigned",
  "float",
  "double",
  "void",
  "long long"
};

string toString(UberModifiers m)
{
  xassert(uberModifierNames[UM_NUM_FLAGS-1] != NULL);
  return bitmapString(m, uberModifierNames, UM_NUM_FLAGS);
}
