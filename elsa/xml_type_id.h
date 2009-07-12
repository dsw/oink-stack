// xml_type_id.h            see license.txt for copyright and terms of use

// identity management for serialization for the type system, template
// system, and variables.

#ifndef XML_TYPE_ID_H
#define XML_TYPE_ID_H

#include "cc_type.h"            // types
#include "variable.h"           // variables
#include "xmlhelp.h"            // xmlUniqueId_t
#include "hashline.h"           // HashLineMap
#include "template.h"           // InheritedTemplateParams

// manage identity; definitions; FIX: I don't like how printed() is
// still using the object address instead of its unique id, but those
// are one-to-one so I suppose its ok for now

class IdentityManager_Elsa {
public:
#define identity_defn0(PREFIX, NAME, TEMPL) \
  TEMPL char const *idPrefix(NAME const * const) {return #PREFIX;} \
  TEMPL xmlUniqueId_t uniqueId(NAME const * const obj) {return mapAddrToUniqueId(obj);} \
  TEMPL bool printed0(NAME const * const obj) {                                \
    if (printedSet ##PREFIX.contains(obj)) return true;                        \
    return false;                                                              \
  }                                                                            \
  TEMPL bool printed(NAME const * const obj) {                                 \
    if (printedSet ##PREFIX.contains(obj)) return true;                        \
    printedSet ##PREFIX.add(obj);                                              \
    return false;                                                              \
  }

#define identity_defn(PREFIX, NAME) identity_defn0(PREFIX, NAME, )
#define identityTempl_defn(PREFIX, NAME) identity_defn0(PREFIX, NAME, template<class T>)

  identity_defn(FI, SourceLocManager::File)
  identity_defn(FI, HashLineMap)
  identity_defn(FI, HashLineMap::HashLine)
  identity_defn(FI, unsigned char) // for lineLengths

  identity_defn(BC, BaseClass)
  identity_defn(TY, Type)
  identity_defn(TY, CompoundType)
  identity_defn(TY, FunctionType::ExnSpec)
  // identity_defn(TY, EnumType)
  identity_defn(TY, EnumType::Value)
  identity_defn(TY, Variable)
  identity_defn(TY, OverloadSet)
  identity_defn(TY, STemplateArgument)
  identity_defn(TY, TemplateInfo)
  identity_defn(TY, InheritedTemplateParams)
  identity_defn(TY, StringRefMap<Variable>)

  identityTempl_defn(OL, ObjList<T>)
  identityTempl_defn(OL, ObjArrayStack<T>)
  identityTempl_defn(OL, SObjList<T>)
  identityTempl_defn(NM, StringRefMap<T>)
  identityTempl_defn(NM, StringObjDict<T>)
  identityTempl_defn(FI, ArrayStack<T>)

#define identityCpdSuper(PREFIX, NAME)                                         \
  char const *idPrefix(NAME const * const obj) {                               \
    if (CompoundType const * const cpd = dynamic_cast<CompoundType const * const>(obj)) { \
      return idPrefix(cpd);                                                    \
    }                                                                          \
    return #PREFIX;                                                            \
  }                                                                            \
    xmlUniqueId_t uniqueId(NAME const * const obj) {                           \
      if (CompoundType const * const cpd = dynamic_cast<CompoundType const * const>(obj)) { \
        return uniqueId(cpd);                                                  \
      }                                                                        \
      return mapAddrToUniqueId(obj);                                           \
    }                                                                          \
    bool printed(NAME const * const obj) {                                     \
      if (CompoundType const * const cpd = dynamic_cast<CompoundType const * const>(obj)) { \
        return printed(cpd);                                                   \
      }                                                                        \
      if (printedSet ##PREFIX.contains(obj)) return true;                      \
      printedSet ##PREFIX.add(obj);                                            \
      return false;                                                            \
    }

  // AtomicType and Scope are special because they both can be a
  // CompoundType sometimes and so have to change their notion of
  // identity when they do
  identityCpdSuper(TY, AtomicType)
  identityCpdSuper(TY, Scope)

protected:
  // printing of types is idempotent
  SObjSet<void const *> printedSetTY;
  SObjSet<void const *> printedSetBC;
  SObjSet<void const *> printedSetOL;
  SObjSet<void const *> printedSetNM;
  SObjSet<void const *> printedSetFI;

};

// the next line is good for replacing with codepatch.pl:
typedef IdentityManager_Elsa IdentityManager_T;

// class IdentityManager is used in cc.ast.gen.cc
class IdentityManager : public IdentityManager_T {};

#undef identity_defn0
#undef identity_defn
#undef identityTempl_defn
#undef identityCpdSuper

#endif
