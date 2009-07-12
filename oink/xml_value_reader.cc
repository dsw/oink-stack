// see License.txt for copyright and terms of use

#include "xml_value_reader.h"   // this module
#include "xmlhelp.h"            // xmlAttrDeQuote
#include "xml_enum.h"           // XTOK_*
#include "oink_util.h"          // USER_ERROR
#include "oink_global.h"        // vFac

void *XmlValueReader::ctorNodeFromTag(int tag) {
  if (void *ret = ctorNodeFromTag_reusable(tag)) return ret;

  switch(tag) {
  default: return NULL; break;
  case 0: xmlUserFatalError("unexpected file termination while looking for an open tag name");

  // **** Values
  case XTOK_CVAtomicValue:        return vFac->alloc_CVAtomicValue();
  case XTOK_PointerValue:         return vFac->alloc_PointerValue();
  case XTOK_ReferenceValue:       return vFac->alloc_ReferenceValue();
  case XTOK_FunctionValue:        return vFac->alloc_FunctionValue();
  case XTOK_ArrayValue:           return vFac->alloc_ArrayValue();
  case XTOK_PointerToMemberValue: return vFac->alloc_PointerToMemberValue();

  // **** Other
  case XTOK_Variable_O: return new Variable_O((XmlReader&)*this);

  // **** Containers
  case XTOK_List_FunctionValue_params: return new SObjList<Variable_O>();
  case XTOK_List_externVars: return new SObjList<Variable_O>();
  }
}

// when I override ctorNodeFromTag(), I want to exclude some tags, but
// others I want subclasses to be able to re-use, so I put the
// handling for them here
void *XmlValueReader::ctorNodeFromTag_reusable(int tag) {
  switch(tag) {
  default: return NULL; break;
  case 0: xfailure("zero tag"); break;

  case XTOK_FVEllipsisHolder: return new FVEllipsisHolder((XmlReader&)*this);

  case XTOK_BucketOValues: return new BucketOValues();

  case XTOK_NameMap_BucketOValues_nameToValue: return new StringRefMap<Value>();
  }
}

bool XmlValueReader::kind2kindCat(int kind, KindCategory *kindCat) {
  switch(kind) {
  default: return false;        // we don't know this kind

  // Values
  case XTOK_CVAtomicValue:
  case XTOK_PointerValue:
  case XTOK_ReferenceValue:
  case XTOK_FunctionValue:
  case XTOK_ArrayValue:
  case XTOK_PointerToMemberValue:
  case XTOK_FVEllipsisHolder:
  // Other
  case XTOK_Variable_O:
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
  case XTOK_BucketOValues:
    *kindCat = KC_Node;
    break;
  }
  return true;
}

bool XmlValueReader::recordKind(int kind, bool& answer) {
  switch(kind) {
  default: return false;        // we don't know this kind

  // **** do not record these
  // Types
  case XTOK_CVAtomicValue:
  case XTOK_PointerValue:
  case XTOK_ReferenceValue:
  case XTOK_FunctionValue:
  case XTOK_ArrayValue:
  case XTOK_PointerToMemberValue:
  case XTOK_FVEllipsisHolder:
  // Other
  case XTOK_Variable_O:
  // **** Containers
  //   SObjList
  case XTOK_List_FunctionValue_params:
  case XTOK_List_externVars:
  case XTOK_BucketOValues:
  case XTOK_NameMap_BucketOValues_nameToValue:
    answer = false;
    return true;
    break;
  }
}

bool XmlValueReader::callOpAssignToEmbeddedObj(void *obj, int kind, void *target) {
  return false;
}

bool XmlValueReader::upcastToWantedType(void *obj, int objKind, void **target, int targetKind) {
  return false;
}

// **************** registerStringToken

bool XmlValueReader::registerStringToken(void *target, int kind, char const *yytext0) {
  return false;
}

// **************** registerAttribute

#define regAttr(TYPE) \
  registerAttr_##TYPE((TYPE*)target, attr, yytext0)

bool XmlValueReader::registerAttribute(void *target, int kind, int attr, char const *yytext0) {
  if (registerAttribute_reusable(target, kind, attr, yytext0)) return true;

  switch(kind) {
  default: return false; break;

  // **** Values
  case XTOK_CVAtomicValue: regAttr(CVAtomicValue);               break;
  case XTOK_PointerValue: regAttr(PointerValue);                 break;
  case XTOK_ReferenceValue: regAttr(ReferenceValue);             break;
  case XTOK_FunctionValue: regAttr(FunctionValue);               break;
  case XTOK_ArrayValue: regAttr(ArrayValue);                     break;
  case XTOK_PointerToMemberValue: regAttr(PointerToMemberValue); break;

  // **** Other
  case XTOK_Variable_O: regAttr(Variable_O);                     break;
  }

  return true;
}

// when I override registerAttribute(), I want to exclude some tags,
// but others I want subclasses to be able to re-use, so I put the
// handling for them here
bool XmlValueReader::registerAttribute_reusable
  (void *target, int kind, int attr, char const *yytext0) {
  switch(kind) {
  default: return false; break;
  case XTOK_FVEllipsisHolder:
    regAttr(FVEllipsisHolder); break;
  case XTOK_BucketOValues:
    regAttr(BucketOValues); break;
  }
  return true;
}

bool XmlValueReader::registerAttr_Value_super(Value *obj, int attr, char const *strValue) {
  switch(attr) {
  default: return false;        // we didn't find it break;
  case XTOK_refToMe:
    // FIX: should this be XTOK_ReferenceValue?
    ul(refToMe, XTOK_Value); break;
  case XTOK_ptrToMe: ul(ptrToMe, XTOK_Value); break;
  case XTOK_loc: fromXml_SourceLoc(obj->loc, strValue); break;
  case XTOK_usedInDataflow:
    xassert(!Value::allow_annotation);
    ValueAnnotation_O *va = (ValueAnnotation_O*) obj->getAnnotation0();
    fromXml_bool(va->usedInDataflow, strValue);
    break;
  }
  return true;                  // found it
}

void XmlValueReader::registerAttr_CVAtomicValue(CVAtomicValue *obj, int attr, char const *strValue) {
  // superclass
  if (registerAttr_Value_super(obj, attr, strValue)) return;
  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a CVAtomicValue"); break;
  case XTOK_type: ul(type, XTOK_Type); break;
  case XTOK_atVoid: ul(atVoid, XTOK_Value); break;
  case XTOK_ufLinkForEquivRelOfDomOfMapToBucket: ul(ufLinkForEquivRelOfDomOfMapToBucket, XTOK_ufLinkForEquivRelOfDomOfMapToBucket); break;
  case XTOK_bucket: ul(bucket, XTOK_bucket); break;
//   case XTOK__repr_value:
//     xfailure("16d693fb-2d86-4a69-956e-7df5b6568401");
//     break;
//   case XTOK__repr_bucket:
//     xfailure("3c7d216f-8496-4eb1-a460-707384384745");
//     break;
  }
}

void XmlValueReader::registerAttr_PointerValue(PointerValue *obj, int attr, char const *strValue) {
  // superclass
  if (registerAttr_Value_super(obj, attr, strValue)) return;
  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a PointerValue"); break;
  case XTOK_type: ul(type, XTOK_Type); break;
  case XTOK_atValue: ul(atValue, XTOK_Value); break;
  }
}

void XmlValueReader::registerAttr_ReferenceValue(ReferenceValue *obj, int attr, char const *strValue) {
  // superclass
  if (registerAttr_Value_super(obj, attr, strValue)) return;
  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a ReferenceValue"); break;
  case XTOK_type: ul(type, XTOK_Type); break;
  case XTOK_atValue: ul(atValue, XTOK_Value); break;
  }
}

void XmlValueReader::registerAttr_FunctionValue(FunctionValue *obj, int attr, char const *strValue) {
  // superclass
  if (registerAttr_Value_super(obj, attr, strValue)) return;
  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a FunctionValue"); break;
  case XTOK_type: ul(type, XTOK_Type); break;
  case XTOK_retValue: ul(retValue, XTOK_Value); break;
  case XTOK_retVar: ul(retVar, XTOK_Variable_O); break;
    // quarl 2006-06-02
    //    Note: 'params' is a pointer to a list, not an embedded list!
  case XTOK_params: ul(params, XTOK_List_FunctionValue_params); break;
  case XTOK_ellipsisHolder: ul(ellipsisHolder, XTOK_FVEllipsisHolder); break;
  }
}

void XmlValueReader::registerAttr_ArrayValue(ArrayValue *obj, int attr, char const *strValue) {
  // superclass
  if (registerAttr_Value_super(obj, attr, strValue)) return;
  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a ArrayValue"); break;
  case XTOK_type: ul(type, XTOK_Type); break;
  case XTOK_atValue: ul(atValue, XTOK_Value); break;
  }
}

void XmlValueReader::registerAttr_PointerToMemberValue
  (PointerToMemberValue *obj, int attr, char const *strValue)
{
  // superclass
  if (registerAttr_Value_super(obj, attr, strValue)) return;
  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a PointerToMemberValue"); break;
  case XTOK_type: ul(type, XTOK_Type); break;
  case XTOK_atValue: ul(atValue, XTOK_Value); break;
  }
}

void XmlValueReader::registerAttr_FVEllipsisHolder
  (FVEllipsisHolder *obj, int attr, char const *strValue)
{
  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a FVEllipsisHolder"); break;
  case XTOK_ellipsis: ul(ellipsis, XTOK_Variable_O); break;
  }
}

void XmlValueReader::registerAttr_Variable_O(Variable_O *obj, int attr, char const *strValue) {
  // superclass
  if (typeXmlReader.registerAttr_Variable_super(obj, attr, strValue)) return;
  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a Variable"); break;
  case XTOK_abstrValue0: ul(abstrValue0, XTOK_Value); break;
  // NOTE: might bring this back, but for now is serialized as user1
  //  case XTOK_hasFuncDefn: fromXml_bool(obj->hasFuncDefn, strValue); break;
  }
}

void XmlValueReader::registerAttr_BucketOValues
  (BucketOValues *obj, int attr, char const *strValue) {
  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a Variable"); break;
  case XTOK_nameToValue:
    ulEmbed(nameToValue, XTOK_NameMap_BucketOValues_nameToValue);
    break;
  }
}
