// see License.txt for copyright and terms of use

#include "oink_global.h"        // this module

bool typecheckingDone = false;
bool elaboratingDone = false;

OinkCmd *oinkCmd = NULL;

CCLang globalLang;

bool anyCplusplus = false;

StringTable globalStrTable;
StringRef strRef__builtin_va_start = globalStrTable("__builtin_va_start");
StringRef strRef__builtin_va_copy  = globalStrTable("__builtin_va_copy");
StringRef strRef__builtin_va_arg   = globalStrTable("__builtin_va_arg");
StringRef strRef__builtin_va_end   = globalStrTable("__builtin_va_end");

#if DEBUG_INSTANCE_SPECIFIC_VALUES
PtrSet<Value> instanceSpecificValues;
PtrSet<Value> dataDeclaratorValues;
#endif

// quarl 2006-05-19 the union-find is now inlined into Value.

int globalNextFunCallId = 0;

ASTPrinter_O *astPrinter = NULL;

// this has no state so we just make it here
ValueTypePrinter typePrinterOink;

TypeFactory_O *tFac = NULL;
ValueFactory *vFac = NULL;

Linker linker;

Controls *controls = NULL;

// NOTE: The containers that contain <char const> actually contain
// StringRef-s, but I can't say that the way those templates are
// parameterized.
SObjSet<StringRef> loadedModules;
// NOTE: should contain char const but that causes problems in the
// implementation of class ASTList: extern ASTList<char const>
// moduleList;
SObjList<char> moduleList;
StringSObjDict<char const> file2module;
StringRef defaultModule = NULL;

PtrSet<char const> *funcFilterNames = NULL;
