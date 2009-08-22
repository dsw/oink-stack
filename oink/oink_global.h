// see License.txt for copyright and terms of use

//  All of Oink's global variables in one place.

#ifndef BASIC_GLOBALS_H
#define BASIC_GLOBALS_H

#include "oink_debug.h"
#include "oink_var.h"
#include "value.h"
#include "oink_cmd.h"
#include "value_print.h"
#include "cc_lang.h"
#include "oink.h"
#include "oink_control.h"
#include "astlist.h"
// #include "Lib/union_find_map.h"

// NOTE: The containers that contain <char const> actually contain
// StringRef-s, but I can't say that the way those templates are
// parameterized.

// so that I can assert in various places that I haven't accidentally
// started any analysis until typechecking was done; must be global
// because those places are everywhere
extern bool typecheckingDone;
// same for elaborating
extern bool elaboratingDone;

// parsed command line; I didn't want to have to create this globally,
// but there is no other way to get it into a few places like the ctor
// of Var_Q.
extern OinkCmd *oinkCmd;

// parsing language options; FIX: this should be different per file
extern CCLang globalLang;

// Whether any files we are analyzing have C++.  This doesn't take into
// account already-serialized files, but that should be OK for now since we'll
// check invariants (such as no isens w/ C++) before serializing, and we'll
// check consistency upon deserializing.
extern bool anyCplusplus;

// string table for storing parse tree identifiers; this is here so
// that I can get to it anywhere
extern StringTable globalStrTable;
// optimization: pre-compute these and keep them around
extern StringRef strRef__builtin_va_start;
extern StringRef strRef__builtin_va_copy;
extern StringRef strRef__builtin_va_arg;
extern StringRef strRef__builtin_va_end;

// quarl 2006-06-22
//    This somewhat-expensive hash table is only used for assertions that we
//    now think are always true, so only enable this for debugging.
#if DEBUG_INSTANCE_SPECIFIC_VALUES
// annotation for a type as instance-specific: (the static versions
// of) instance-specific types are not allowed to be used in the
// dataflow analysis when instance-sensitive is on.
extern PtrSet<Value> instanceSpecificValues;
// a declarator type is also an instance-specific type
extern PtrSet<Value> dataDeclaratorValues;
#endif

// quarl 2006-05-19 (The cva2bov union-find is now inlined into Value.)

// a source for unique id-s for function call sites
extern int globalNextFunCallId;

// a class for printing out stuff about ast nodes with virtual methods
// that can be overriden by a client backend analysis without oink
// having to know about them
class ASTPrinter_O;
extern ASTPrinter_O *astPrinter;

// a singleton class for printing out types
extern ValueTypePrinter typePrinterOink;

// global factory pointer
extern TypeFactory_O *tFac;
extern ValueFactory *vFac;

// global map from the fully qualified extern names to the extern
// variables; this needs to be global
extern Linker linker;

// user controls
extern Controls *controls;

// set-semantics list of modules
extern SObjList<char> moduleList;
// map from filenames to modulenames
extern StringSObjDict<char const> file2module;

// StringRefs of names in func_filter file; this is global because it
// makes writing a callback function in Oink convenient
extern PtrSet<char const> *funcFilterNames;

#endif // BASIC_GLOBALS_H
