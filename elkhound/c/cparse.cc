// cparse.cc            see license.txt for copyright and terms of use
// code for cparse.h

#include <iostream.h>    // cout

#include "cparse.h"      // this module
#include "cc_lang.h"     // CCLang
#include "trace.h"       // trace

ParseEnv::ParseEnv(StringTable &table, CCLang &L)
  : str(table), 
    intType(table.add("int")),
    strRefAttr(table.add("attr")),
    types(),
    lang(L)
{}

ParseEnv::~ParseEnv()
{}


static char const *identity(void *data)
{
  return (char const*)data;
}

void ParseEnv::enterScope()
{
  types.prepend(new StringHash(identity));
}

void ParseEnv::leaveScope()
{
  delete types.removeAt(0);
}

void ParseEnv::addType(StringRef type)
{
  StringHash *h = types.first();
  if (h->get(type)) {
    // this happens for C++ code which has both the implicit
    // and explicit typedefs (and/or, explicit 'class Foo' mentions
    // in places)
    //cout << "duplicate entry for " << type << " -- will ignore\n";
  }
  else {
    h->add(type, (void*)type);
  }
}

bool ParseEnv::isType(StringRef name)
{
  if (name == intType) {
    return true;
  }

  FOREACH_OBJLIST(StringHash, types, iter) {
    if (iter.data()->get(name)) {
      return true;
    }
  }
  return false;
}


void ParseEnv::declareClassTag(StringRef tagName)
{
  // TYPE/NAME
  if (lang.tagsAreTypes) {
    #ifndef NDEBUG
    trace("cc") << "defined new struct/class tag as type " << tagName << endl;
    #endif
    addType(tagName);
  }
}
