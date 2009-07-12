// c_variable.cc            see license.txt for copyright and terms of use
// code for c_variable.h

#include "c_variable.h"    // this module
#include "c_type.h"        // Type


// ---------------------- Variable --------------------
Variable::Variable(SourceLoc L, StringRef n, Type const *t, DeclFlags f)
  : loc(L),
    name(n),
    type(t),
    flags(f)
{
  xassert(type);        // (just a stab in the dark debugging effort)
}

Variable::~Variable()
{}


string Variable::toString() const
{
  // The purpose of this method is to print the name and type
  // of this Variable object, in a debugging context.  It is
  // not necessarily intended to print them in a way consistent
  // with the C syntax that might give rise to the Variable.
  // If more specialized printing is desired, do that specialized
  // printing from outside (by directly accessing 'name', 'type',
  // 'flags', etc.).
  return type->toCString(name? name : "");
}
