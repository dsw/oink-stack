// see License.txt for copyright and terms of use

#include "qual_xml_value_reader.h" // this module
#include "qual_value_children.h" // *Value_Q
#include "oink_global.h"

void *XmlValueReader_Q::ctorNodeFromTag(int tag) {
  // check if it is one of the reusable tags from the superclass
  if (void *ret = ctorNodeFromTag_reusable(tag)) return ret;

  switch(tag) {
  default: return NULL; break;
  case 0: xmlUserFatalError("unexpected file termination while looking for an open tag name");

  // **** Values

    // quarl 2006-06-20
    //    The vFac should be allocating
    //    Struct<CVAtomicValue_Q,CVAtomicValue_QualAnnot>, etc.
  case XTOK_CVAtomicValue_Q:        return vFac->alloc_CVAtomicValue();
  case XTOK_PointerValue_Q:         return vFac->alloc_PointerValue();
  case XTOK_ReferenceValue_Q:       return vFac->alloc_ReferenceValue();
  case XTOK_FunctionValue_Q:        return vFac->alloc_FunctionValue();
  case XTOK_ArrayValue_Q:           return vFac->alloc_ArrayValue();
  case XTOK_PointerToMemberValue_Q: return vFac->alloc_PointerToMemberValue();

  // **** Other
  case XTOK_Variable_Q: return new Variable_Q((XmlReader&)*this);

  // **** Containers
  // SObjList
  case XTOK_List_FunctionValue_params: return new SObjList<Variable_Q>();
  case XTOK_List_externVars: return new SObjList<Variable_Q>();
  }
}

bool XmlValueReader_Q::kind2kindCat(int kind, KindCategory *kindCat) {
  switch(kind) {
  default: return false;        // we don't know this kind

  // Values
  case XTOK_CVAtomicValue_Q:
  case XTOK_PointerValue_Q:
  case XTOK_ReferenceValue_Q:
  case XTOK_FunctionValue_Q:
  case XTOK_ArrayValue_Q:
  case XTOK_PointerToMemberValue_Q:
  // Other
  case XTOK_Variable_Q:
  case XTOK_FVEllipsisHolder:
  case XTOK_BucketOValues:
    *kindCat = KC_Node;
    break;

  // **** Containers
  //   SObjList
  case XTOK_List_FunctionValue_params:
  case XTOK_List_externVars:
    *kindCat = KC_SObjList;
    break;
  //   StringSObjDict
  case XTOK_NameMap_BucketOValues_nameToValue:
    *kindCat = KC_StringSObjDict;
    break;
  }
  return true;
}

bool XmlValueReader_Q::recordKind(int kind, bool& answer) {
  switch(kind) {
  default: return false;        // we don't know this kind
  // **** do not record these
  // Types
  case XTOK_CVAtomicValue_Q:
  case XTOK_PointerValue_Q:
  case XTOK_ReferenceValue_Q:
  case XTOK_FunctionValue_Q:
  case XTOK_ArrayValue_Q:
  case XTOK_PointerToMemberValue_Q:
  // Other
  case XTOK_Variable_Q:
  // **** Containers
  //   SObjList
  case XTOK_List_FunctionValue_params:
  case XTOK_List_externVars:

  // **** duplicated from superclass
  case XTOK_FVEllipsisHolder:
  //   StringRefMap
  case XTOK_NameMap_BucketOValues_nameToValue:
  case XTOK_BucketOValues:
    answer = false;
    return true;
    break;
  }
}

// **************** registerAttribute

#define regAttr(TYPE) \
  registerAttr_##TYPE((TYPE*)target, attr, yytext0)

// NOTE!  We register attributes using the superclass method; this is
// because none of the *_Q classes have any extra state than their
// corresponding oink versions.
bool XmlValueReader_Q::registerAttribute(void *target, int kind, int attr, char const *yytext0) {
  // check if it is one of the resuable tags from the superclass
  if (registerAttribute_reusable(target, kind, attr, yytext0)) return true;

  switch(kind) {
  default: return false; break;

  // **** Values
  case XTOK_CVAtomicValue_Q: regAttr(CVAtomicValue);               break;
  case XTOK_PointerValue_Q: regAttr(PointerValue);                 break;
  case XTOK_ReferenceValue_Q: regAttr(ReferenceValue);             break;
  case XTOK_FunctionValue_Q: regAttr(FunctionValue);               break;
  case XTOK_ArrayValue_Q: regAttr(ArrayValue);                     break;
  case XTOK_PointerToMemberValue_Q: regAttr(PointerToMemberValue); break;

  // **** Other
  case XTOK_Variable_Q: regAttr(Variable_O);                       break;
  }

  return true;
}
