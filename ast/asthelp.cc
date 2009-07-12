// asthelp.cc            see license.txt for copyright and terms of use
// code for what's declared in asthelp.h

#include "asthelp.h"       // this module
#include "strutil.h"       // quoted
#include "exc.h"           // xformat


// ----------- debugPrint helpers -----------------------
ostream &ind(ostream &os, int indent)
{
  while (indent--) {
    os << " ";
  }
  return os;
}


void debugPrintStr(string const &s, char const *name,
                   ostream &os, int indent)
{
  debugPrintStr(s.c_str(), name, os, indent);
}

void debugPrintStr(char const *s, char const *name,
                   ostream &os, int indent)
{
  string s1((s) ? string(s) : string("NULL"));
  ind(os, indent) << name << " = " << quoted(s1) << "\n";
}


void debugPrintCStr(char const *s, char const *name,
                    ostream &os, int indent)
{
  ind(os, indent) << name << " = ";
  if (s) {
    os << quoted(s);
  }
  else {
    os << "(null)";
  }
  os << "\n";
}


template <class STR>
void debugPrintStringList(ASTList<STR> const &list, char const *name,
                          ostream &os, int indent)
{
  ind(os, indent) << name << ": ";
  {
    int ct=0;
    FOREACH_ASTLIST(STR, list, iter) {
      if (ct++ > 0) {
        os << ", ";
      }
      os << quoted(string(*( iter.data() )));
    }
  }
  os << "\n";
}


void debugPrintList(ASTList<string> const &list, char const *name,
                    ostream &os, int indent)
{
  debugPrintStringList(list, name, os, indent);
}

void debugPrintList(ASTList<LocString> const &list, char const *name,
                    ostream &os, int indent)
{
  debugPrintStringList(list, name, os, indent);
}


// EOF
