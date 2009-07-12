// ia64mangle.cc
// code for ia64mangle.h

#include "ia64mangle.h"      // this module

// elsa
#include "cc_type.h"         // Type
#include "variable.h"        // Variable

// smbase
#include "strutil.h"         // replace


string ia64MangleType(Type *t)
{
  // for now, hack it; this is not IA64 at all
  string s = t->toString();

  // translation table to get valid identifiers
  static struct S {
    char const *src;
    char const *dest;
  } const map[] = {
    { "<", "__LT" },
    { ">", "__GT" },
    { "(", "__LP" },
    { ")", "__RP" },
    { "*", "__p" },
    { " ", "" },
    { "&", "__r" },
    { "::", "__cc" },
    { ",", "__co" },
  };

  for (int i=0; i < TABLESIZE(map); i++) {
    s = replace(s, map[i].src, map[i].dest);
  }

  return s;
}


// EOF
