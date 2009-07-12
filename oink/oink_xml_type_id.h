// see License.txt for copyright and terms of use

// This file is #included into xml_type_id.h by xml_type_id.h.cpatch.

// It would make sense if astgen just generated the IdentityManager directly.

// Another possibility is to have a single virtual function which switches on
// type.

// #include "Lib/union_find_map.h" // UnionFindMap
#include "value.h"
#include "oink_var.h"

#ifndef XML_TYPE_ID_H
# error do not use this except through xml_type_id.h.cpatch
#endif

// typedef UnionFindMap<CVAtomicValue, BucketOValues> UFM_CVA_BOV;
// typedef PtrMap<CVAtomicValue, BucketOValues> PM_CVA_BOV;
// typedef UnionFind<CVAtomicValue> UF_CVA;
// typedef PtrMap<CVAtomicValue, CVAtomicValue> PM_CVA_CVA;

class IdentityManager_Oink : public IdentityManager_Elsa {
public:

  identity_defn(VL, QLiterals);
  identity_defn(VL, Value);
  identity_defn(VL, FVEllipsisHolder);

// I can't do this because the preprocessor doesn't know that
// angle-brackets hide the comma
// identity_decl(VL, UnionFindMap<CVAtomicValue, BucketOValues>);

  // identity_defn(VL, UFM_CVA_BOV);
  // identity_defn(VL, PM_CVA_BOV);
  // identity_defn(VL, UF_CVA);
  // identity_defn(VL, PM_CVA_CVA);
  identity_defn(VL, BucketOValues);

  identityTempl_defn(OL, StringSObjDict<T>);

// We handle Variable and Variable_O together now so don't do this:
// identity_decl(Variable_O);

  // below are copied from elsa/xml_type_id.h

  identity_defn(FI, SourceLocManager::File)
  identity_defn(FI, HashLineMap)
  identity_defn(FI, HashLineMap::HashLine)
  identity_defn(FI, unsigned char) // for lineLengths

  identity_defn(BC, BaseClass)
  identity_defn(TY, Type)
  identity_defn(TY, CompoundType)
  identity_defn(TY, FunctionType::ExnSpec)
  identity_defn(TY, EnumType::Value)
  identity_defn(TY, Variable)
  identity_defn(TY, OverloadSet)
  identity_defn(TY, STemplateArgument)
  identity_defn(TY, TemplateInfo)
  identity_defn(TY, InheritedTemplateParams)
  identity_defn(TY, StringRefMap<Variable>)

  identityCpdSuper(TY, AtomicType)
  identityCpdSuper(TY, Scope)

  identityTempl_defn(OL, ObjList<T>)
  identityTempl_defn(OL, ObjArrayStack<T>)
  identityTempl_defn(OL, SObjList<T>)
  identityTempl_defn(OL, TailList<T>)
  identityTempl_defn(NM, StringRefMap<T>)
  identityTempl_defn(NM, StringObjDict<T>)
  identityTempl_defn(FI, ArrayStack<T>)

protected:
  SObjSet<void const *> printedSetVL;
};

typedef IdentityManager_Oink IdentityManager_T;

