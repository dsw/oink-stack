// cc_flags.cc            see license.txt for copyright and terms of use
// code for cc_flags.h

#include "cc_flags.h"     // this module
#include "macros.h"       // STATIC_ASSERT
#include "xassert.h"      // xassert
#include "trace.h"        // tracingSys
#include "strtokpc.h"     // StrtokParseC
#include "exc.h"          // xformat
#include <ctype.h>        // toupper


// the check for array[limit-1] is meant to ensure that there
// are as many specified entries as there are total entries
#define MAKE_TOSTRING(T, limit, array)        \
  char const *toString(T index)               \
  {                                           \
    xassert((unsigned)index < limit);         \
    xassert(array[limit-1] != NULL);          \
    return array[index];                      \
  }


// given a table like {"a","bb","ccc"} and prefix "P", produce a table
// like {"PA","PBB","PCCC"}
char **buildPrefixUppercaseMap(char const *prefix, char const * const *src,
                               int numEntries)
{
  // table itself
  char **ret = new char*[numEntries];

  // fill the entries
  for (int i=0; i<numEntries; i++) {
    // prefix, then src[i]
    int len = strlen(prefix)+strlen(src[i]);
    ret[i] = new char[len+1];
    strcpy(ret[i], prefix);
    strcat(ret[i], src[i]);

    // uppercase
    for (int j=0; j<len; j++) {
      ret[i][j] = toupper(ret[i][j]);
    }
  }

  return ret;
}


// find the index of a string in 'names' that equals 'str', or
// throw xFormat on error
int findInMap(char const * const *names, int numNames,
              char const *kind, char const *str)
{
  for (int i=0; i<numNames; i++) {
    if (0==strcmp(names[i], str)) {
      return i;
    }
  }
  xformat(stringc << "unknown " << kind << ": " << str);
  return 0;    // silence warning
}


// -------------------- TypeIntr -------------------------
char const * const typeIntrNames[NUM_TYPEINTRS] = {
  "struct",
  "class",
  "union",
  "enum"
};

MAKE_TOSTRING(TypeIntr, NUM_TYPEINTRS, typeIntrNames)

static char const * const *typeIntrPrefixNames()
{
  static char const * const *names = 0;
  if (!names) {
    names = buildPrefixUppercaseMap("TI_", typeIntrNames, NUM_TYPEINTRS);
  }
  return names;
}

char const *toXml(TypeIntr id)
{
  xassert((unsigned)id < (unsigned)NUM_TYPEINTRS);
  return typeIntrPrefixNames()[id];
}

void fromXml(TypeIntr &out, char const *str)
{
  out = (TypeIntr)findInMap(typeIntrPrefixNames(), NUM_TYPEINTRS,
                            "TypeIntr", str);
}


// ---------------- CVFlags -------------
char const * const cvFlagNames[NUM_CVFLAGS] = {
  "const",
  "volatile",
  "restrict",
  "owner"
};


string bitmapString(int bitmap, char const * const *names, int numFlags,
                    char const *delim)
{
  // make sure I haven't added a flag without adding a string for it
  xassert(names[numFlags-1] != NULL);

  stringBuilder sb;
  int count=0;
  for (int i=0; i<numFlags; i++) {
    if (bitmap & (1 << i)) {
      if (count++) {
        sb << delim;
      }
      sb << names[i];
    }
  }

  return sb;
}

string toString(CVFlags cv)
{
  return bitmapString(cv >> CV_SHIFT_AMOUNT, cvFlagNames, NUM_CVFLAGS, " ");
}

string toXml(CVFlags id)
{
  return bitmapString(id >> CV_SHIFT_AMOUNT, cvFlagNames, NUM_CVFLAGS, "|");
}


// Given the string 'str' which contains several space-delimited
// flag names drawn from 'names', return a bitmap that consists
// of the corresponding flag bits.
//
// For best performance, 'str' should have its flags in numerically
// increasing order (this routine does not sort them first).
int fromBitmapString(char const * const *names, int numFlags,
                     char const *kind, char const *str, char delim)
{
  StrtokParseC tok(str);

  int ret = 0;       // set of flags that have been found in 'str'
  int tokIndex = 0;  // progress in 'tok'

  char const *curToken = tok.nextToken(delim);

  // loop while still flag names in 'tok' yet to be processed
  while (curToken != NULL) {
    int origTokIndex = tokIndex;

    // loop over names in 'names'
    for (int flag = 0; flag<numFlags; flag++) {
      if (0==strcmp(names[flag], curToken)) {
        // the current string is the current flag
        ret |= (1 << flag);
        tokIndex++;
        curToken = tok.nextToken(delim);
        if (curToken == NULL) break;
      }
    }

    // made progress?
    if (origTokIndex == tokIndex) {
      // failed to make progress; this string isn't anywhere
      // in the 'names' array
      xformat(stringc << "unknown " << kind << ": " << curToken);
    }
  }

  return ret;
}

void fromXml(CVFlags &out, char const *str)
{
  int tmp = fromBitmapString(cvFlagNames, NUM_CVFLAGS, "CVFlag", str, '|');
  out = (CVFlags)(tmp << CV_SHIFT_AMOUNT);
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

  "(enumerator)",
  "(global)",
  "(initialized)",
  "(builtin)",
  "(bound tparam)", // 14
  "(addrtaken)",
  "(parameter)",
  "(universal)",
  "(existential)",
  "(member)",       // 19
  "(definition)",
  "(inline_defn)",
  "(implicit)",
  "(forward)",
  "(temporary)",    // 24

  "(unused)",
  "namespace",
  "(extern \"C\")",
  "(selfname)",
  "(templ param)",  // 29
  "(using alias)",
  "(bitfield)",
};


string toString(DeclFlags df)
{
  return bitmapString(df, declFlagNames, NUM_DECLFLAGS, " ");
}

string toXml(DeclFlags id)
{
  return bitmapString(id, declFlagNames, NUM_DECLFLAGS, "|");
}

void fromXml(DeclFlags &out, char const *str)
{
  out = (DeclFlags)fromBitmapString(declFlagNames, NUM_DECLFLAGS,
                                    "DeclFlag", str, '|');
}


// ----------------------- ScopeKind ----------------------------
char const *toString(ScopeKind sk)
{
  static char const * const arr[] = {
    "unknown",
    "global",
    "parameter",
    "function",
    "class",
    "template_params",
    "template_args",
    "namespace",
  };
  STATIC_ASSERT(TABLESIZE(arr) == NUM_SCOPEKINDS);

  xassert((unsigned)sk < NUM_SCOPEKINDS);
  return arr[sk];
}


// ---------------------- SimpleTypeId --------------------------
bool isValid(SimpleTypeId id)
{
  return 0 <= id && id <= NUM_SIMPLE_TYPES;
}


#define S(x) ((SimpleTypeFlags)(x))    // work around bitwise-OR in initializers..
static SimpleTypeInfo const simpleTypeInfoArray[] = {
  //name                   size,    flags
  { "char",                   1,    S(STF_INTEGER                          ) },
  { "unsigned char",          1,    S(STF_INTEGER | STF_UNSIGNED           ) },
  { "signed char",            1,    S(STF_INTEGER                          ) },
  { "bool",                   4,    S(STF_INTEGER                          ) },
  { "int",                    4,    S(STF_INTEGER | STF_PROM               ) },
  { "unsigned int",           4,    S(STF_INTEGER | STF_PROM | STF_UNSIGNED) },
  { "long int",               4,    S(STF_INTEGER | STF_PROM               ) },
  { "unsigned long int",      4,    S(STF_INTEGER | STF_PROM | STF_UNSIGNED) },
  { "long long int",          8,    S(STF_INTEGER | STF_PROM               ) },
  { "unsigned long long int", 8,    S(STF_INTEGER | STF_PROM | STF_UNSIGNED) },
  { "short int",              2,    S(STF_INTEGER                          ) },
  { "unsigned short int",     2,    S(STF_INTEGER | STF_UNSIGNED           ) },
  { "wchar_t",                2,    S(STF_INTEGER                          ) },
  { "float",                  4,    S(STF_FLOAT                            ) },
  { "double",                 8,    S(STF_FLOAT | STF_PROM                 ) },
  { "long double",           10,    S(STF_FLOAT                            ) },
  { "float _Complex",         8,    S(STF_FLOAT                            ) },
  { "double _Complex",       16,    S(STF_FLOAT                            ) },
  { "long double _Complex",  20,    S(STF_FLOAT                            ) },
  { "float _Imaginary",       4,    S(STF_FLOAT                            ) },
  { "double _Imaginary",      8,    S(STF_FLOAT                            ) },
  { "long double _Imaginary",10,    S(STF_FLOAT                            ) },
  { "void",                   1,    S(STF_NONE                             ) },    // gnu: sizeof(void) is 1

  // these should go away early on in typechecking
  { "...",                    0,    S(STF_NONE                             ) },
  { "/*cdtor*/",              0,    S(STF_NONE                             ) },    // dsw: don't want to print <cdtor>
  { "(error)",                0,    S(STF_NONE                             ) },
  { "(dependent)",            0,    S(STF_NONE                             ) },
  { "(implicit-int)",         0,    S(STF_NONE                             ) },
  { "(notfound)",             0,    S(STF_NONE                             ) },


  { "(prom_int)",             0,    S(STF_NONE                             ) },
  { "(prom_arith)",           0,    S(STF_NONE                             ) },
  { "(integral)",             0,    S(STF_NONE                             ) },
  { "(arith)",                0,    S(STF_NONE                             ) },
  { "(arith_nobool)",         0,    S(STF_NONE                             ) },
  { "(any_obj)",              0,    S(STF_NONE                             ) },
  { "(non_void)",             0,    S(STF_NONE                             ) },
  { "(any_type)",             0,    S(STF_NONE                             ) },


  { "(pret_strip_ref)",       0,    S(STF_NONE                             ) },
  { "(pret_ptm)",             0,    S(STF_NONE                             ) },
  { "(pret_arith_conv)",      0,    S(STF_NONE                             ) },
  { "(pret_first)",           0,    S(STF_NONE                             ) },
  { "(pret_first_ptr2ref)",   0,    S(STF_NONE                             ) },
  { "(pret_second)",          0,    S(STF_NONE                             ) },
  { "(pret_second_ptr2ref)",  0,    S(STF_NONE                             ) },
};
#undef S

SimpleTypeInfo const &simpleTypeInfo(SimpleTypeId id)
{
  STATIC_ASSERT(TABLESIZE(simpleTypeInfoArray) == NUM_SIMPLE_TYPES);
  xassert(isValid(id));
  return simpleTypeInfoArray[id];
}


bool isComplexOrImaginary(SimpleTypeId id)
{
  return ST_FLOAT_COMPLEX <= id && id <= ST_DOUBLE_IMAGINARY;
}


#define MAKE_USUAL_ENUM_TO_FROM_XML(TYPE, numElements)            \
  char const *toXml(TYPE id)                                      \
  {                                                               \
    return toString(id);   /* overloaded */                       \
  }                                                               \
                                                                  \
  void fromXml(TYPE &out, char const *str)                        \
  {                                                               \
    for (int id=0; id < numElements; id++) {                      \
      TYPE typedId = (TYPE)id;                                    \
      if (0==strcmp(toString(typedId), str)) {                    \
        out = typedId;                                            \
        return;                                                   \
      }                                                           \
    }                                                             \
    xfailure(stringc << "bad " #TYPE ": '" << str << "'");        \
  }

// Some of the strings have angle brackets.  But, those cases should
// not appear in the tree that gets serialized.
MAKE_USUAL_ENUM_TO_FROM_XML(SimpleTypeId, NUM_SIMPLE_TYPES);


// ------------------------ UnaryOp -----------------------------
char const * const unaryOpNames[NUM_UNARYOPS] = {
  "+",
  "-",
  "!",
  "~"
};

MAKE_TOSTRING(UnaryOp, NUM_UNARYOPS, unaryOpNames)
MAKE_USUAL_ENUM_TO_FROM_XML(UnaryOp, NUM_UNARYOPS);


// SGM 2007-09-04: I removed the "/*postfix*/" stuff because it is
// mostly clutter at this point.  If it is desired again I will
// create a new 'toStringWithComments' or something.
char const * const effectOpNames[NUM_EFFECTOPS] = {
  "++",
  "--",
  "++",
  "--",
};

MAKE_TOSTRING(EffectOp, NUM_EFFECTOPS, effectOpNames)
MAKE_USUAL_ENUM_TO_FROM_XML(EffectOp, NUM_EFFECTOPS);

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
  ",",

  "<?",
  ">?",

  "[]",

  "=",

  ".*",
  "->*",

  "==>",
  "<==>",
};

MAKE_TOSTRING(BinaryOp, NUM_BINARYOPS, binaryOpNames)
MAKE_USUAL_ENUM_TO_FROM_XML(BinaryOp, NUM_BINARYOPS);

bool isPredicateCombinator(BinaryOp op)
{
  return op==BIN_AND || op==BIN_OR || op==BIN_IMPLIES || op==BIN_EQUIVALENT;
}

bool isRelational(BinaryOp op)
{
  return BIN_EQUAL <= op && op <= BIN_GREATEREQ;
}

bool isInequality(BinaryOp op)
{
  return BIN_LESS <= op && op <= BIN_GREATEREQ;
}

bool isOverloadable(BinaryOp op)
{
  return BIN_EQUAL <= op && op <= BIN_BRACKETS ||
         op == BIN_ARROW_STAR;
}


// ------------------- AccessKeyword -------------------
char const * const accessKeywordNames[NUM_ACCESS_KEYWORDS] = {
  "public",
  "protected",
  "private",
  "unspecified"
};

MAKE_TOSTRING(AccessKeyword, NUM_ACCESS_KEYWORDS, accessKeywordNames)
MAKE_USUAL_ENUM_TO_FROM_XML(AccessKeyword, NUM_ACCESS_KEYWORDS);


// -------------------- CastKeyword --------------------
char const * const castKeywordNames[NUM_CAST_KEYWORDS] = {
  "dynamic_cast",
  "static_cast",
  "reinterpret_cast",
  "const_cast"
};

MAKE_TOSTRING(CastKeyword, NUM_CAST_KEYWORDS, castKeywordNames)
MAKE_USUAL_ENUM_TO_FROM_XML(CastKeyword, NUM_CAST_KEYWORDS);


// -------------------- OverloadableOp --------------------
char const * const overloadableOpNames[NUM_OVERLOADABLE_OPS] = {
  "!",
  "~",

  "++",
  "--",

  "+",
  "-",
  "*",
  "&",

  "/",
  "%",
  "<<",
  ">>",
  "^",
  "|",

  "=",
  "+=",
  "-=",
  "*=",
  "/=",
  "%=",
  "<<=",
  ">>=",
  "&=",
  "^=",
  "|=",

  "==",
  "!=",
  "<",
  ">",
  "<=",
  ">=",

  "&&",
  "||",

  "->",
  "->*",

  "[]",
  "()",
  ",",
  "?:",

  "<?",
  ">?",
};

MAKE_TOSTRING(OverloadableOp, NUM_OVERLOADABLE_OPS, overloadableOpNames)
MAKE_USUAL_ENUM_TO_FROM_XML(OverloadableOp, NUM_OVERLOADABLE_OPS);


char const * const operatorFunctionNames[NUM_OVERLOADABLE_OPS] = {
  "operator!",
  "operator~",

  "operator++",
  "operator--",

  "operator+",
  "operator-",
  "operator*",
  "operator&",

  "operator/",
  "operator%",
  "operator<<",
  "operator>>",
  "operator^",
  "operator|",

  "operator=",
  "operator+=",
  "operator-=",
  "operator*=",
  "operator/=",
  "operator%=",
  "operator<<=",
  "operator>>=",
  "operator&=",
  "operator^=",
  "operator|=",

  "operator==",
  "operator!=",
  "operator<",
  "operator>",
  "operator<=",
  "operator>=",

  "operator&&",
  "operator||",

  "operator->",
  "operator->*",

  "operator[]",
  "operator()",
  "operator,",
  "operator?",

  "operator<?",
  "operator>?",
};


OverloadableOp toOverloadableOp(UnaryOp op)
{
  static OverloadableOp const map[] = {
    OP_PLUS,
    OP_MINUS,
    OP_NOT,
    OP_BITNOT
  };
  ASSERT_TABLESIZE(map, NUM_UNARYOPS);
  xassert(validCode(op));
  return map[op];
}

OverloadableOp toOverloadableOp(EffectOp op)
{
  static OverloadableOp const map[] = {
    OP_PLUSPLUS,
    OP_MINUSMINUS,
    OP_PLUSPLUS,
    OP_MINUSMINUS,
  };
  ASSERT_TABLESIZE(map, NUM_EFFECTOPS);
  xassert(validCode(op));
  return map[op];
}

OverloadableOp toOverloadableOp(BinaryOp op, bool isAssignment)
{
  xassert(validCode(op));

  // in the table, this means that an operator cannot be overloaded
  #define BAD_ENTRY NUM_OVERLOADABLE_OPS
  OverloadableOp ret = BAD_ENTRY;

  if (!isAssignment) {
    static OverloadableOp const map[] = {
      OP_EQUAL,
      OP_NOTEQUAL,
      OP_LESS,
      OP_GREATER,
      OP_LESSEQ,
      OP_GREATEREQ,

      OP_STAR,
      OP_DIV,
      OP_MOD,
      OP_PLUS,
      OP_MINUS,
      OP_LSHIFT,
      OP_RSHIFT,
      OP_AMPERSAND,
      OP_BITXOR,
      OP_BITOR,
      OP_AND,
      OP_OR,
      OP_COMMA,

      OP_MINIMUM,
      OP_MAXIMUM,

      OP_BRACKETS,

      BAD_ENTRY,      // isAssignment is false

      BAD_ENTRY,      // cannot overload
      OP_ARROW_STAR,

      BAD_ENTRY,      // extension..
      BAD_ENTRY,
    };
    ASSERT_TABLESIZE(map, NUM_BINARYOPS);
    ret = map[op];
  }

  else {
    static OverloadableOp const map[] = {
      BAD_ENTRY,
      BAD_ENTRY,
      BAD_ENTRY,
      BAD_ENTRY,
      BAD_ENTRY,
      BAD_ENTRY,

      OP_MULTEQ,
      OP_DIVEQ,
      OP_MODEQ,
      OP_PLUSEQ,
      OP_MINUSEQ,
      OP_LSHIFTEQ,
      OP_RSHIFTEQ,
      OP_BITANDEQ,
      OP_BITXOREQ,
      OP_BITOREQ,
      BAD_ENTRY,
      BAD_ENTRY,
      BAD_ENTRY,

      BAD_ENTRY,
      BAD_ENTRY,

      BAD_ENTRY,

      OP_ASSIGN,

      BAD_ENTRY,
      BAD_ENTRY,

      BAD_ENTRY,
      BAD_ENTRY,
    };
    ASSERT_TABLESIZE(map, NUM_BINARYOPS);
    ret = map[op];
  }

  xassert(ret != BAD_ENTRY);    // otherwise why did you try to map it?
  return ret;

  #undef BAD_ENTRY
}

// ------------------------ UberModifiers ---------------------
char const * const uberModifierNames[UM_NUM_FLAGS] = {
  "auto",            // 0x00000001
  "register",
  "static",
  "extern",
  "mutable",         // 0x00000010
  "inline",
  "virtual",
  "explicit",
  "friend",          // 0x00000100
  "typedef",

  "const",
  "volatile",
  "restrict",        // 0x00001000

  "wchar_t",
  "bool",
  "short",
  "int",             // 0x00010000
  "long",
  "signed",
  "unsigned",
  "float",           // 0x00100000
  "double",
  "void",
  "long long",
  "char",            // 0x01000000
  "complex",
  "imaginary"
};

string toString(UberModifiers m)
{
  xassert(uberModifierNames[UM_NUM_FLAGS-1] != NULL);
  return bitmapString(m, uberModifierNames, UM_NUM_FLAGS, " ");
}


// ---------------------- SpecialExpr -----------------
#define CASE(name) case name: return #name

char const *toString(SpecialExpr se)
{
  switch (se) {
    default: xfailure("bad se code");
    CASE(SE_NONE);
    CASE(SE_ZERO);
    CASE(SE_STRINGLIT);
  }
}


// ---------------- TemplateParameterKind -------------
char const *toString(TemplateParameterKind tpk)
{
  switch (tpk) {
    default: xfailure("bad tpk code");
    CASE(TPK_TYPE);
    CASE(TPK_NON_TYPE);
    CASE(TPK_TEMPLATE);
  }
}


// EOF
