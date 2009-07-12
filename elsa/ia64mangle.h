// ia64mangle.h
// IA64 name mangler

// The algorithm is not really tied to the IA64 ("Itanium")
// architecture, it is just specified with the IA64 ABI and hence has
// the value of being somewhat standard.

#ifndef IA64MANGLE_H
#define IA64MANGLE_H

// smbase
#include "str.h"             // string

// fwds
class Type;                  // cc_type.h
class Variable;              // variable.h

string ia64MangleType(Type *t);
string ia64MangleVariable(Variable *v);

#endif // IA64MANGLE_H
