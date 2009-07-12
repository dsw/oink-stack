// cc_flags.h            see license.txt for copyright and terms of use
// enumerated flags for parsing C

#ifndef CC_FLAGS_H
#define CC_FLAGS_H

#include "str.h"     // string

// ----------------------- TypeIntr ----------------------
// type introducer keyword
// NOTE: keep consistent with CompoundType::Keyword (cc_type.h)
enum TypeIntr {
  TI_STRUCT,
  TI_CLASS,
  TI_UNION,
  TI_ENUM,
  NUM_TYPEINTRS
};

extern char const * const typeIntrNames[NUM_TYPEINTRS];    // "struct", ...
string toString(TypeIntr tr);


// --------------------- CVFlags ---------------------
// set: which of "const" and/or "volatile" is specified;
// I leave the lower 8 bits to represent SimpleTypeId, so I can
// freely OR them together during parsing;
// values in common with UberModifier must line up
enum CVFlags {
  CV_NONE     = 0x0000,
  CV_CONST    = 0x0400,
  CV_VOLATILE = 0x0800,
  CV_OWNER    = 0x1000,     // experimental extension
  CV_ALL      = 0x1C00,

  CV_SHIFT_AMOUNT = 10,     // shift right this many bits before counting for cvFlagNames
  NUM_CVFLAGS = 3           // # bits set to 1 in CV_ALL
};

extern char const * const cvFlagNames[NUM_CVFLAGS];      // 0="const", 1="volatile", 2="owner"
string toString(CVFlags cv);

inline CVFlags operator| (CVFlags f1, CVFlags f2)
  { return (CVFlags)((int)f1 | (int)f2); }
inline CVFlags& operator|= (CVFlags &f1, CVFlags f2)
  { return f1 = f1 | f2; }


// ----------------------- DeclFlags ----------------------
// set of declaration modifiers present;
// these modifiers apply to variable names;
// they're now also being used for Variable (variable.h) flags;
// values in common with UberModifiers must line up
enum DeclFlags {
  DF_NONE        = 0x00000000,

  // syntactic declaration modifiers
  DF_AUTO        = 0x00000001,
  DF_REGISTER    = 0x00000002,
  DF_STATIC      = 0x00000004,
  DF_EXTERN      = 0x00000008,
  DF_MUTABLE     = 0x00000010,
  DF_INLINE      = 0x00000020,
  DF_VIRTUAL     = 0x00000040,
  DF_EXPLICIT    = 0x00000080,
  DF_FRIEND      = 0x00000100,
  DF_TYPEDEF     = 0x00000200,
  DF_SOURCEFLAGS = 0x000003FF,    // all flags that come from keywords in the source

  // flags on Variables
  DF_ENUMERATOR  = 0x00000400,    // true for values in an 'enum' (enumerators in the terminology of the C++ standard)
  DF_GLOBAL      = 0x00000800,    // set for globals, unset for locals
  DF_INITIALIZED = 0x00001000,    // true if has been declared with an initializer (or, for functions, with code)
  DF_BUILTIN     = 0x00002000,    // true for e.g. __builtin_constant_p -- don't emit later
  DF_LOGIC       = 0x00004000,    // true for logic variables
  DF_ADDRTAKEN   = 0x00008000,    // true if it's address has been (or can be) taken
  DF_PARAMETER   = 0x00010000,    // true if this is a function parameter
  DF_UNIVERSAL   = 0x00020000,    // (requires DF_LOGIC) universally-quantified variable
  DF_EXISTENTIAL = 0x00040000,    // (requires DF_LOGIC) existentially-quantified
  DF_MEMBER      = 0x00080000,    // true for members of classes (data, static data, functions)
  DF_DEFINITION  = 0x00100000,    // set once we've seen this Variable's definition
  DF_INLINE_DEFN = 0x00200000,    // set for inline function definitions on second pass of tcheck
  DF_IMPLICIT    = 0x00400000,    // set for C++ implicit typedefs
  DF_FORWARD     = 0x00800000,    // for syntax which only provides a forward declaration

  // syntactic declaration extensions
  DF_PREDICATE   = 0x01000000,    // Simplify-declared predicate (i.e. DEFPRED)

  ALL_DECLFLAGS  = 0x01FFFFFF,
  NUM_DECLFLAGS  = 25             // # bits set to 1 in ALL_DECLFLAGS
};

extern char const * const declFlagNames[NUM_DECLFLAGS];      // 0="inline", 1="virtual", 2="friend", ..
string toString(DeclFlags df);

inline DeclFlags operator| (DeclFlags f1, DeclFlags f2)
  { return (DeclFlags)((int)f1 | (int)f2); }
inline DeclFlags& operator|= (DeclFlags &f1, DeclFlags f2)
  { return f1 = f1 | f2; }
inline DeclFlags operator& (DeclFlags f1, DeclFlags f2)
  { return (DeclFlags)((int)f1 & (int)f2); }
inline DeclFlags operator~ (DeclFlags f)
  { return (DeclFlags)((~(int)f) & ALL_DECLFLAGS); }

// ------------------------- SimpleTypeId ----------------------------
// C's built-in scalar types; the representation deliberately does
// *not* imply any orthogonality of properties (like long vs signed);
// separate query functions can determine such properties, or signal
// when it is meaningless to query a given property of a given type
// (like whether a floating-point type is unsigned)
enum SimpleTypeId {
  ST_CHAR,
  ST_UNSIGNED_CHAR,
  ST_SIGNED_CHAR,
  ST_BOOL,
  ST_INT,
  ST_UNSIGNED_INT,
  ST_LONG_INT,
  ST_UNSIGNED_LONG_INT,
  ST_LONG_LONG,                      // GNU extension
  ST_UNSIGNED_LONG_LONG,             // GNU extension
  ST_SHORT_INT,
  ST_UNSIGNED_SHORT_INT,
  ST_WCHAR_T,
  ST_FLOAT,
  ST_DOUBLE,
  ST_LONG_DOUBLE,
  ST_VOID,
  ST_ELLIPSIS,                       // used to encode vararg functions
  ST_CDTOR,                          // "return type" for ctors and dtors
  ST_ERROR,                          // this type is returned for typechecking errors
  ST_DEPENDENT,                      // depdenent on an uninstantiated template parameter type
  NUM_SIMPLE_TYPES,
  ST_BITMASK = 0xFF                  // for extraction for OR with CVFlags
};

// info about each simple type
struct SimpleTypeInfo {
  char const *name;       // e.g. "unsigned char"
  int reprSize;           // # of bytes to store
  bool isInteger;         // ST_INT, etc., but not e.g. ST_FLOAT
};

bool isValid(SimpleTypeId id);                          // bounds check
SimpleTypeInfo const &simpleTypeInfo(SimpleTypeId id);

inline char const *simpleTypeName(SimpleTypeId id)
  { return simpleTypeInfo(id).name; }
inline int simpleTypeReprSize(SimpleTypeId id)
  { return simpleTypeInfo(id).reprSize; }
inline string toString(SimpleTypeId id)
  { return string(simpleTypeName(id)); }


// ---------------------------- UnaryOp ---------------------------
enum UnaryOp {
  UNY_PLUS,      // +
  UNY_MINUS,     // -
  UNY_NOT,       // !
  UNY_BITNOT,    // ~
  NUM_UNARYOPS
};

extern char const * const unaryOpNames[NUM_UNARYOPS];     // "+", ...
string toString(UnaryOp op);


// unary operator with a side effect
enum EffectOp {
  EFF_POSTINC,   // ++ (postfix)
  EFF_POSTDEC,   // -- (postfix)
  EFF_PREINC,    // ++
  EFF_PREDEC,    // --
  NUM_EFFECTOPS
};

extern char const * const effectOpNames[NUM_EFFECTOPS];   // "++", ...
string toString(EffectOp op);
bool isPostfix(EffectOp op);


// ------------------------ BinaryOp --------------------------
enum BinaryOp {
  // the relationals come first, and in this order, to correspond
  // to RelationOp in predicate.ast
  BIN_EQUAL,     // ==
  BIN_NOTEQUAL,  // !=
  BIN_LESS,      // <
  BIN_GREATER,   // >
  BIN_LESSEQ,    // <=
  BIN_GREATEREQ, // >=

  BIN_MULT,      // *
  BIN_DIV,       // /
  BIN_MOD,       // %
  BIN_PLUS,      // +
  BIN_MINUS,     // -
  BIN_LSHIFT,    // <<
  BIN_RSHIFT,    // >>
  BIN_BITAND,    // &
  BIN_BITXOR,    // ^
  BIN_BITOR,     // |
  BIN_AND,       // &&
  BIN_OR,        // ||

  BIN_ASSIGN,    // = (used to denote simple assignments in AST, as opposed to (say) "+=")

  // C++ operators
  BIN_DOT_STAR,    // .*
  BIN_ARROW_STAR,  // ->*

  // theorem prover extension
  BIN_IMPLIES,   // ==>

  NUM_BINARYOPS
};

extern char const * const binaryOpNames[NUM_BINARYOPS];   // "*", ..
string toString(BinaryOp op);

bool isPredicateCombinator(BinaryOp op);     // &&, ||, ==>
bool isRelational(BinaryOp op);              // == thru >=


// ---------------- access control ------------
enum AccessKeyword {
  AK_PUBLIC,
  AK_PROTECTED,
  AK_PRIVATE,
  AK_UNSPECIFIED,      // not explicitly specified; typechecking changes it later
  
  NUM_ACCESS_KEYWORDS
};

extern char const * const accessKeywordNames[NUM_ACCESS_KEYWORDS];
string toString(AccessKeyword key);

// ---------------- cast keywords -------------
enum CastKeyword {
  CK_DYNAMIC,
  CK_STATIC,
  CK_REINTERPRET,
  CK_CONST,

  NUM_CAST_KEYWORDS
};

extern char const * const castKeywordNames[NUM_CAST_KEYWORDS];
string toString(CastKeyword key);


// --------------- overloadable operators --------
// these are just the operators that are overloadable
// but aren't already listed in one of the lists above
enum OverloadableOp {
  OVL_COMMA,      // ,
  OVL_ARROW,      // ->
  OVL_PARENS,     // ( )
  OVL_BRACKETS,   // [ ]

  NUM_OVERLOADABLE_OPS
};

extern char const * const overloadableOpNames[NUM_OVERLOADABLE_OPS];
string toString(OverloadableOp op);


// -------------------- uber modifiers -----------------
// the uber modifiers are a superset of all the keywords which
// can appear in a type specifier; see cc.gr, nonterm DeclSpecifier
enum UberModifiers {
  UM_NONE         = 0,

  // decl flags
  UM_AUTO         = 0x00000001,
  UM_REGISTER     = 0x00000002,
  UM_STATIC       = 0x00000004,
  UM_EXTERN       = 0x00000008,
  UM_MUTABLE      = 0x00000010,

  UM_INLINE       = 0x00000020,
  UM_VIRTUAL      = 0x00000040,
  UM_EXPLICIT     = 0x00000080,

  UM_FRIEND       = 0x00000100,
  UM_TYPEDEF      = 0x00000200,

  UM_DECLFLAGS    = 0x000003FF,

  // cv-qualifier
  UM_CONST        = 0x00000400,
  UM_VOLATILE     = 0x00000800,

  UM_CVFLAGS      = 0x00000C00,

  // type keywords
  UM_CHAR         = 0x00001000,
  UM_WCHAR_T      = 0x00002000,
  UM_BOOL         = 0x00004000,
  UM_SHORT        = 0x00008000,
  UM_INT          = 0x00010000,
  UM_LONG         = 0x00020000,
  UM_SIGNED       = 0x00040000,
  UM_UNSIGNED     = 0x00080000,
  UM_FLOAT        = 0x00100000,
  UM_DOUBLE       = 0x00200000,
  UM_VOID         = 0x00400000,
  UM_LONG_LONG    = 0x00800000,    // GNU extension

  UM_TYPEKEYS     = 0x00FFF000,

  UM_ALL_FLAGS    = 0x00FFFFFF,
  UM_NUM_FLAGS    = 24             // # bits set in UM_ALL_FLAGS
};

// string repr.
extern char const * const uberModifierNames[UM_NUM_FLAGS];
string toString(UberModifiers m);

// select particular subsets
inline DeclFlags uberDeclFlags(UberModifiers m)
  { return (DeclFlags)(m & UM_DECLFLAGS); }
inline CVFlags uberCVFlags(UberModifiers m)
  { return (CVFlags)(m & UM_CVFLAGS); }

// two more related functions, uberSimpleType and uberCombine,
// are declared in ccparse.h

// I do *not* define operators to combine the flags with | and &
// because I want those operations to always be done by dedicated
// functions like 'uberDeclFlags'


#endif // CC_FLAGS_H
