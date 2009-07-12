// xml_type_reader.cc            see license.txt for copyright and terms of use

#include "xml_type_reader.h"    // this module
#include "variable.h"           // Variable
#include "strtokpc.h"           // StrtokParseC
#include "cc_flags.h"           // fromXml(DeclFlags &out, rostring str)
#include "xmlhelp.h"            // fromXml_int() etc.
#include "xml_enum.h"           // XTOK_*


// fromXml for enums

void fromXml(CompoundType::Keyword &out, char const *str) {
  if(0) xfailure("?");
  READENUM(CompoundType::K_STRUCT);
  READENUM(CompoundType::K_CLASS);
  READENUM(CompoundType::K_UNION);
  else xfailure("bad enum string");
}

void fromXml(FunctionFlags &out, char const *str) {
  StrtokParseC tok(str);
  char const *token;
  while ( (token = tok.nextToken('|')) != NULL ) {
    if(0) xfailure("?");
    READFLAG(FF_NONE);
    READFLAG(FF_METHOD);
    READFLAG(FF_VARARGS);
    READFLAG(FF_CONVERSION);
    READFLAG(FF_CTOR);
    READFLAG(FF_DTOR);
    READFLAG(FF_BUILTINOP);
    READFLAG(FF_NO_PARAM_INFO);
    READFLAG(FF_DEFAULT_ALLOC);
    READFLAG(FF_KANDR_DEFN);
    else xfailure("illegal flag");
  }
}

void fromXml(ScopeKind &out, char const *str) {
  if(0) xfailure("?");
  READENUM(SK_UNKNOWN);
  READENUM(SK_GLOBAL);
  READENUM(SK_PARAMETER);
  READENUM(SK_FUNCTION);
  READENUM(SK_CLASS);
  READENUM(SK_TEMPLATE_PARAMS);
  READENUM(SK_TEMPLATE_ARGS);
  READENUM(SK_NAMESPACE);
  else xfailure("bad enum string");
}

void fromXml(STemplateArgument::Kind &out, char const *str) {
  if(0) xfailure("?");
  READENUM(STemplateArgument::STA_NONE);
  READENUM(STemplateArgument::STA_TYPE);
  READENUM(STemplateArgument::STA_INT);
  READENUM(STemplateArgument::STA_ENUMERATOR);
  READENUM(STemplateArgument::STA_REFERENCE);
  READENUM(STemplateArgument::STA_POINTER);
  READENUM(STemplateArgument::STA_MEMBER);
  READENUM(STemplateArgument::STA_DEPEXPR);
  READENUM(STemplateArgument::STA_TEMPLATE);
  READENUM(STemplateArgument::STA_ATOMIC);
  else xfailure("bad enum string");
}


bool XmlTypeReader::kind2kindCat(int kind, KindCategory *kindCat) {
  switch(kind) {
  default: return false;        // we don't know this kind

  // Types
  case XTOK_CVAtomicType:
  case XTOK_PointerType:
  case XTOK_ReferenceType:
  case XTOK_FunctionType:
  case XTOK_FunctionType_ExnSpec:
  case XTOK_ArrayType:
  case XTOK_PointerToMemberType:
  case XTOK_DependentSizedArrayType:
  // AtomicTypes
  case XTOK_SimpleType:
  case XTOK_CompoundType:
  case XTOK_EnumType:
  case XTOK_EnumType_Value:
  case XTOK_TypeVariable:
  case XTOK_PseudoInstantiation:
  case XTOK_DependentQType:
  // Other
  case XTOK_Variable:
  case XTOK_Scope:
  case XTOK_BaseClass:
  case XTOK_BaseClassSubobj:
  case XTOK_OverloadSet:
  case XTOK_STemplateArgument:
  case XTOK_TemplateInfo:
  case XTOK_InheritedTemplateParams:
    *kindCat = KC_Node;
    break;

  // **** Containers

  //   ObjList
  case XTOK_List_CompoundType_bases:
  case XTOK_List_CompoundType_virtualBases:
  case XTOK_List_PseudoInstantiation_args:
  case XTOK_List_TemplateInfo_inheritedParams:
  case XTOK_List_TemplateInfo_arguments:
  case XTOK_List_TemplateInfo_argumentsToPrimary:
  case XTOK_List_PQ_qualifier_sargs:
  case XTOK_List_PQ_template_sargs:
    *kindCat = KC_ObjList;
    break;

  //   SObjList
  case XTOK_List_FunctionType_params:
  case XTOK_List_CompoundType_dataMembers:
  case XTOK_List_CompoundType_conversionOperators:
  case XTOK_List_BaseClassSubobj_parents:
  case XTOK_List_ExnSpec_types:
  case XTOK_List_Scope_templateParams:
  case XTOK_List_OverloadSet_set:
  case XTOK_List_TemplateInfo_instantiations:
  case XTOK_List_TemplateInfo_specializations:
  case XTOK_List_TemplateInfo_partialInstantiations:
  case XTOK_List_TemplateParams_params:
    *kindCat = KC_SObjList;
    break;

  //   StringRefMap
  case XTOK_NameMap_Scope_variables:
  case XTOK_NameMap_Scope_typeTags:
    *kindCat = KC_StringRefMap;
    break;

  //   StringSObjDict
  case XTOK_NameMap_EnumType_valueIndex:
    *kindCat = KC_StringSObjDict;
    break;
  }
  return true;
}

bool XmlTypeReader::recordKind(int kind, bool& answer) {
  switch(kind) {
  default: return false;        // we don't know this kind

  // **** record these kinds
  case XTOK_CompoundType:
    answer = true;
    return true;
    break;

  // **** do not record these

  // Types
  case XTOK_CVAtomicType:
  case XTOK_PointerType:
  case XTOK_ReferenceType:
  case XTOK_FunctionType:
  case XTOK_FunctionType_ExnSpec:
  case XTOK_ArrayType:
  case XTOK_PointerToMemberType:
  case XTOK_DependentSizedArrayType:
  // AtomicTypes
  case XTOK_SimpleType:
//    case XTOK_CompoundType: handled above
  case XTOK_EnumType:
  case XTOK_EnumType_Value:
  case XTOK_TypeVariable:
  case XTOK_PseudoInstantiation:
  case XTOK_DependentQType:
  // Other
  case XTOK_Variable:
  case XTOK_Scope:
  case XTOK_BaseClass:
  case XTOK_BaseClassSubobj:
  case XTOK_OverloadSet:
  case XTOK_STemplateArgument:
  case XTOK_TemplateInfo:
  case XTOK_InheritedTemplateParams:
  // **** Containers
  //   ObjList
  case XTOK_List_CompoundType_bases:
  case XTOK_List_CompoundType_virtualBases:
  case XTOK_List_PseudoInstantiation_args:
  case XTOK_List_TemplateInfo_inheritedParams:
  case XTOK_List_TemplateInfo_arguments:
  case XTOK_List_TemplateInfo_argumentsToPrimary:
  case XTOK_List_PQ_qualifier_sargs:
  case XTOK_List_PQ_template_sargs:
  //   SObjList
  case XTOK_List_FunctionType_params:
  case XTOK_List_CompoundType_dataMembers:
  case XTOK_List_CompoundType_conversionOperators:
  case XTOK_List_BaseClassSubobj_parents:
  case XTOK_List_ExnSpec_types:
  case XTOK_List_Scope_templateParams:
  case XTOK_List_OverloadSet_set:
  case XTOK_List_TemplateInfo_instantiations:
  case XTOK_List_TemplateInfo_specializations:
  case XTOK_List_TemplateInfo_partialInstantiations:
  case XTOK_List_TemplateParams_params:
  //   StringRefMap
  case XTOK_NameMap_Scope_variables:
  case XTOK_NameMap_Scope_typeTags:
  //   StringSObjDict
  case XTOK_NameMap_EnumType_valueIndex:
    answer = false;
    return true;
    break;
  }
}

bool XmlTypeReader::callOpAssignToEmbeddedObj(void *obj, int kind, void *target) {
  xassert(obj);
  xassert(target);
  switch(kind) {

  default:
    // This handler conflates two situations; one is where kind is a
    // kind from the typesystem, but not an XTOK_BaseClassSubobj,
    // which is an error; the other is where kind is just not from the
    // typesystem, which should just be a return false so that another
    // XmlReader will be attempted.  However the first situation
    // should not be handled by any of the other XmlReaders either and
    // so should also result in an error, albeit perhaps not as exact
    // of one as it could have been.  I just don't want to put a huge
    // switch statement here for all the other kinds in the type
    // system.
    return false;
    break;

  case XTOK_BaseClassSubobj:
    BaseClassSubobj *obj1 = reinterpret_cast<BaseClassSubobj*>(obj);
    BaseClassSubobj *target1 = reinterpret_cast<BaseClassSubobj*>(target);
    // fprintf(stderr,
    //         "## embed XTOK_BaseClassSubobj: obj1=%p, &obj1->parents=%p, target1=%p, &target1->parents=%p\n",
    //         obj1, &obj1->parents, target1, &target1->parents);
    target1->operator=(*obj1);
    return true;
    break;

  }
}

bool XmlTypeReader::upcastToWantedType(void *obj, int objKind, void **target, int targetKind) {
  xassert(obj);
  xassert(target);

  // classes where something interesting happens
  if (objKind == XTOK_CompoundType) {
    CompoundType *tmp = reinterpret_cast<CompoundType*>(obj);
    if (targetKind == XTOK_CompoundType) {
      *target = tmp;
    } else if (targetKind == XTOK_Scope) {
      // upcast to a Scope
      *target = static_cast<Scope*>(tmp);
    } else if (targetKind == XTOK_AtomicType) {
      // upcast to an AtomicType
      *target = static_cast<AtomicType*>(tmp);
    } else if (targetKind == XTOK_NamedAtomicType) {
      // upcast to an NamedAtomicType
      *target = static_cast<NamedAtomicType*>(tmp);
    }
    return true;
  } else {
    // This handler conflates two situations; one is where objKind is
    // a kind from the typesystem, but not an XTOK_CompoundType, which
    // is an error; the other is where objKind is just not from the
    // typesystem, which should just be a return false so that another
    // XmlReader will be attempted.  However the first situation
    // should not be handled by any of the other XmlReaders either and
    // so should also result in an error, albeit perhaps not as exact
    // of one as it could have been.  I just don't want to put a huge
    // switch statement here for all the other kinds in the type
    // system.
    return false;
  }
}

void *XmlTypeReader::ctorNodeFromTag(int tag) {
  switch(tag) {
  default: return NULL; break;
  case 0: xmlUserFatalError("unexpected file termination while looking for an open tag name");

  // **** Types
  case XTOK_CVAtomicType: return new CVAtomicType((AtomicType*)0, (CVFlags)0);
  case XTOK_PointerType: return new PointerType((CVFlags)0, (Type*)0);
  case XTOK_ReferenceType: return new ReferenceType((Type*)0);
  case XTOK_FunctionType: return new FunctionType((Type*)0);
  case XTOK_FunctionType_ExnSpec: return new FunctionType::ExnSpec();
  case XTOK_ArrayType: return new ArrayType((XmlReader&)*this); // call the special ctor
  case XTOK_PointerToMemberType:
    return new PointerToMemberType((XmlReader&)*this); // call the special ctor
  case XTOK_DependentSizedArrayType: 
    return new DependentSizedArrayType((XmlReader&)*this);

  // **** Atomic Types
  // NOTE: this really should go through the SimpleType::fixed array
  case XTOK_SimpleType: return new SimpleType((SimpleTypeId)0);
  case XTOK_CompoundType: return new CompoundType((CompoundType::Keyword)0, (StringRef)0);
  case XTOK_EnumType: return new EnumType((StringRef)0);
  case XTOK_EnumType_Value:
    return new EnumType::Value((StringRef)0, (EnumType*)0, (int)0, (Variable*)0);
  case XTOK_TypeVariable: return new TypeVariable((StringRef)0);
  case XTOK_PseudoInstantiation: return new PseudoInstantiation((CompoundType*)0);
  case XTOK_DependentQType: return new DependentQType((NamedAtomicType*)0);

  // **** Other
  case XTOK_Variable: return new Variable((XmlReader&)*this);// call the special ctor
  case XTOK_Scope: return new Scope((XmlReader&)*this); // call the special ctor
  case XTOK_BaseClass: return new BaseClass((CompoundType*)0, (AccessKeyword)0, (bool)0);
  case XTOK_BaseClassSubobj:
    // NOTE: special; FIX: should I make the BaseClass on the heap and
    // then delete it?  I'm not sure if the compiler is going to be
    // able to tell that even though it is passed by reference to the
    // BaseClassSubobj that it is not kept there and therefore can be
    // deleted at the end of the full expression.
    return new BaseClassSubobj(BaseClass((CompoundType*)0, (AccessKeyword)0, (bool)0));
  case XTOK_OverloadSet: return new OverloadSet();
  case XTOK_STemplateArgument: return new STemplateArgument();
  case XTOK_TemplateInfo: return new TemplateInfo((SourceLoc)0);
  case XTOK_InheritedTemplateParams: return new InheritedTemplateParams((CompoundType*)0);

  // **** Containers
  // ObjList
  case XTOK_List_CompoundType_bases:
    return new ObjList<BaseClass>();
  case XTOK_List_CompoundType_virtualBases:
    return new ObjList<BaseClassSubobj>();
  case XTOK_List_TemplateInfo_inheritedParams:
    return new ObjList<InheritedTemplateParams>();
  case XTOK_List_TemplateInfo_arguments:
  case XTOK_List_TemplateInfo_argumentsToPrimary:
  case XTOK_List_PseudoInstantiation_args:
  case XTOK_List_PQ_qualifier_sargs:
  case XTOK_List_PQ_template_sargs:
    return new ObjList<STemplateArgument>();

  // SObjList
  case XTOK_List_BaseClassSubobj_parents:
    return new SObjList<BaseClassSubobj>();
  case XTOK_List_ExnSpec_types:
    return new SObjList<Type>();
  case XTOK_List_FunctionType_params:
  case XTOK_List_CompoundType_dataMembers:
  case XTOK_List_CompoundType_conversionOperators:
  case XTOK_List_Scope_templateParams:
  case XTOK_List_OverloadSet_set:
  case XTOK_List_TemplateInfo_instantiations:
  case XTOK_List_TemplateInfo_specializations:
  case XTOK_List_TemplateInfo_partialInstantiations:
  case XTOK_List_TemplateParams_params:
    return new SObjList<Variable>();

  // StringRefMap
  case XTOK_NameMap_Scope_variables:
  case XTOK_NameMap_Scope_typeTags:
    return new StringRefMap<Variable>();
  case XTOK_NameMap_EnumType_valueIndex:
    return new StringObjDict<EnumType::Value>();
  }
}

// **************** registerStringToken

bool XmlTypeReader::registerStringToken(void *target, int kind, char const *yytext0) {
  return false;
}

// **************** registerAttribute

#define regAttr(TYPE) \
  registerAttr_##TYPE((TYPE*)target, attr, yytext0)

bool XmlTypeReader::registerAttribute(void *target, int kind, int attr, char const *yytext0) {
  switch(kind) {
  default: return false; break;

  // **** Types
  case XTOK_CVAtomicType: regAttr(CVAtomicType);               break;
  case XTOK_PointerType: regAttr(PointerType);                 break;
  case XTOK_ReferenceType: regAttr(ReferenceType);             break;
  case XTOK_FunctionType: regAttr(FunctionType);               break;
  case XTOK_ArrayType: regAttr(ArrayType);                     break;
  case XTOK_PointerToMemberType: regAttr(PointerToMemberType); break;
  case XTOK_DependentSizedArrayType: regAttr(DependentSizedArrayType); break;
  case XTOK_FunctionType_ExnSpec:
    registerAttr_FunctionType_ExnSpec
      ((FunctionType::ExnSpec*)target, attr, yytext0);         break;

  // **** Atomic Types
  case XTOK_SimpleType: regAttr(SimpleType);                   break;
  case XTOK_CompoundType: regAttr(CompoundType);               break;
  case XTOK_EnumType: regAttr(EnumType);                       break;
  case XTOK_TypeVariable: regAttr(TypeVariable);               break;
  case XTOK_PseudoInstantiation: regAttr(PseudoInstantiation); break;
  case XTOK_DependentQType: regAttr(DependentQType);           break;
  case XTOK_EnumType_Value:
    registerAttr_EnumType_Value
      ((EnumType::Value*)target, attr, yytext0);               break;

  // **** Other
  case XTOK_Variable: regAttr(Variable);                       break;
  case XTOK_Scope: regAttr(Scope);                             break;
  case XTOK_BaseClass: regAttr(BaseClass);                     break;
  case XTOK_BaseClassSubobj: regAttr(BaseClassSubobj);         break;
  case XTOK_OverloadSet: regAttr(OverloadSet);                 break;
  case XTOK_STemplateArgument: regAttr(STemplateArgument);     break;
  case XTOK_TemplateInfo: regAttr(TemplateInfo);               break;
  case XTOK_InheritedTemplateParams:
    regAttr(InheritedTemplateParams);                          break;
  }

  return true;
}

void XmlTypeReader::registerAttr_CVAtomicType(CVAtomicType *obj, int attr, char const *strValue) {
  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a CVAtomicType"); break;
  case XTOK_atomic: ul(atomic, XTOK_AtomicType); break;
  case XTOK_cv: fromXml(obj->cv, strValue); break;
  }
}

void XmlTypeReader::registerAttr_PointerType(PointerType *obj, int attr, char const *strValue) {
  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a PointerType"); break;
  case XTOK_cv: fromXml(obj->cv, strValue); break;
  case XTOK_atType: ul(atType, XTOK_Type); break;
  }
}

void XmlTypeReader::registerAttr_ReferenceType(ReferenceType *obj, int attr, char const *strValue) {
  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a ReferenceType"); break;
  case XTOK_atType: ul(atType, XTOK_Type); break;
  }
}

void XmlTypeReader::registerAttr_FunctionType(FunctionType *obj, int attr, char const *strValue) {
  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a FunctionType"); break;
  case XTOK_flags: fromXml(obj->flags, strValue); break;
  case XTOK_retType: ul(retType, XTOK_Type); break;
  case XTOK_params: ulEmbed(params, XTOK_List_FunctionType_params); break;
  case XTOK_exnSpec: ul(exnSpec, XTOK_FunctionType_ExnSpec); break;
  }
}

void XmlTypeReader::registerAttr_FunctionType_ExnSpec
  (FunctionType::ExnSpec *obj, int attr, char const *strValue) {
  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a FunctionType_ExnSpec"); break;
  case XTOK_types: ulEmbed(types, XTOK_List_ExnSpec_types); break;
  }
}

void XmlTypeReader::registerAttr_ArrayType(ArrayType *obj, int attr, char const *strValue) {
  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a ArrayType"); break;
  case XTOK_eltType: ul(eltType, XTOK_Type); break;
  case XTOK_size: fromXml_int(obj->size, strValue); break;
  }
}

void XmlTypeReader::registerAttr_PointerToMemberType
  (PointerToMemberType *obj, int attr, char const *strValue) {
  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a PointerToMemberType"); break;
  case XTOK_inClassNAT:
    ul(inClassNAT, XTOK_NamedAtomicType); break;
  case XTOK_cv: fromXml(obj->cv, strValue); break;
  case XTOK_atType: ul(atType, XTOK_Type); break;
  }
}

void XmlTypeReader::registerAttr_DependentSizedArrayType
  (DependentSizedArrayType *obj, int attr, char const *strValue) 
{
  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a DependentSizedArrayType"); break;
  case XTOK_eltType: ul(eltType, XTOK_Type); break;
  case XTOK_sizeExpr: ul(sizeExpr, XTOK_Expression); break;
  }
}

bool XmlTypeReader::registerAttr_Variable_super(Variable *obj, int attr, char const *strValue) {
  switch(attr) {
  default: return false; break; // we didn't find it
  case XTOK_loc: fromXml_SourceLoc(obj->loc, strValue); break;
  case XTOK_name: obj->name = manager->strTable(strValue); break;
  case XTOK_type: ul(type, XTOK_Type); break;
  case XTOK_flags:
    fromXml(const_cast<DeclFlags&>(obj->flags), strValue); break;
  case XTOK_value: ul(value, XTOK_Expression); break;
  case XTOK_defaultParamType: ul(defaultParamType, XTOK_Type); break;
  case XTOK_funcDefn: ul(funcDefn, XTOK_Function); break;
  case XTOK_overload: ul(overload, XTOK_OverloadSet); break;
  case XTOK_scope: ul(scope, XTOK_Scope); break;

  // these fields are abstractions; however here we pretend they are
  // real
  case XTOK_access:
    AccessKeyword access;
    fromXml(access, strValue);
    obj->setAccess(access);
    break;
  case XTOK_real:
    bool real;
    fromXml_bool(real, strValue);
    obj->setReal(real);
    break;
  case XTOK_maybeUsedAsAlias:
    bool maybeUsedAsAlias;
    fromXml_bool(maybeUsedAsAlias, strValue);
    obj->setMaybeUsedAsAlias(maybeUsedAsAlias);
    break;
  case XTOK_user1:
    bool user1;
    fromXml_bool(user1, strValue);
    obj->setUser1(user1);
    break;
  case XTOK_user2:
    bool user2;
    fromXml_bool(user2, strValue);
    obj->setUser2(user2);
    break;
  case XTOK_scopeKind:
    ScopeKind scopeKind;
    fromXml(scopeKind, strValue);
    obj->setScopeKind(scopeKind);
    break;
  case XTOK_hasValue:
    bool hasValue;
    fromXml_bool(hasValue, strValue);
    obj->setHasValue(hasValue);
    break;
  case XTOK_parameterOrdinal:
    int parameterOrdinal;
    fromXml_int(parameterOrdinal, strValue);
    obj->setParameterOrdinal(parameterOrdinal);
    break;

  case XTOK_fullyQualifiedMangledName:
    // FIX: For now throw it away; I suppose perhaps we should check
    // that it is indeed the fully qualified name, but we would have
    // to be done de-serializing the whole object heirarcy first and
    // we aren't even done de-serializing the Variable object.
    break;

  case XTOK_usingAlias_or_parameterizedEntity:
    ul(usingAlias_or_parameterizedEntity, XTOK_Variable); break;
  case XTOK_templInfo: ul(templInfo, XTOK_TemplateInfo); break;
  }

  return true;                  // found it
}

void XmlTypeReader::registerAttr_Variable(Variable *obj, int attr, char const *strValue) {
  // "superclass": just re-use our own superclass code for ourself
  if (registerAttr_Variable_super(obj, attr, strValue)) return;
  // shouldn't get here
  xmlUserFatalError("illegal attribute for a Variable");
}

bool XmlTypeReader::registerAttr_NamedAtomicType_super
  (NamedAtomicType *obj, int attr, char const *strValue) {
  switch(attr) {
  default: return false; break; // we didn't find it
  case XTOK_name: obj->name = manager->strTable(strValue); break;
  case XTOK_typedefVar: ul(typedefVar, XTOK_Variable); break;
  case XTOK_access: fromXml(obj->access, strValue); break;
  }
  return true;                  // found it
}

void XmlTypeReader::registerAttr_SimpleType(SimpleType *obj, int attr, char const *strValue) {
  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a SimpleType"); break;
  case XTOK_type:
    // NOTE: this 'type' is not a type node, but basically an enum,
    // and thus is handled more like a flag would be.
    fromXml(const_cast<SimpleTypeId&>(obj->type), strValue);
    break;
  }
}

void XmlTypeReader::registerAttr_CompoundType(CompoundType *obj, int attr, char const *strValue) {
  // superclasses
  if (registerAttr_NamedAtomicType_super(obj, attr, strValue)) return;
  if (registerAttr_Scope_super(obj, attr, strValue)) return;

  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a CompoundType"); break;
  case XTOK_forward: obj->forward = fromXml_bool(strValue); break;
  case XTOK_keyword: fromXml(obj->keyword, strValue); break;
  case XTOK_dataMembers: ulEmbed(dataMembers, XTOK_List_CompoundType_dataMembers); break;
  case XTOK_bases: ulEmbed(bases, XTOK_List_CompoundType_bases); break;
  case XTOK_virtualBases: ulEmbed(virtualBases, XTOK_List_CompoundType_virtualBases); break;
  case XTOK_subobj: ulEmbed(subobj, XTOK_BaseClassSubobj); break;
  case XTOK_conversionOperators:
    ulEmbed(conversionOperators, XTOK_List_CompoundType_conversionOperators); break;
  case XTOK_instName: obj->instName = manager->strTable(strValue); break;
  case XTOK_syntax: ul(syntax, XTOK_TS_classSpec); break;
  case XTOK_parameterizingScope: ul(parameterizingScope, XTOK_Scope); break;
  case XTOK_selfType: ul(selfType, XTOK_Type); break;
  }
}

void XmlTypeReader::registerAttr_EnumType(EnumType *obj, int attr, char const *strValue) {
  // superclass
  if (registerAttr_NamedAtomicType_super(obj, attr, strValue)) return;

  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a EnumType"); break;
  case XTOK_valueIndex: ulEmbed(valueIndex, XTOK_NameMap_EnumType_valueIndex); break;
  case XTOK_nextValue: fromXml_int(obj->nextValue, strValue); break;
  case XTOK_hasNegativeValues: fromXml_bool(obj->hasNegativeValues, strValue); break;
  }
}

void XmlTypeReader::registerAttr_EnumType_Value
  (EnumType::Value *obj, int attr, char const *strValue) {
  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a EnumType"); break;
  case XTOK_name: obj->name = manager->strTable(strValue); break;
  case XTOK_type: ul(type, XTOK_EnumType); break; // NOTE: 'type' here is actually an atomic type
  case XTOK_value: fromXml_int(obj->value, strValue); break;
  case XTOK_decl: ul(decl, XTOK_Variable); break;
  }
}

void XmlTypeReader::registerAttr_TypeVariable(TypeVariable *obj, int attr, char const *strValue) {
  // superclass
  if (registerAttr_NamedAtomicType_super(obj, attr, strValue)) return;
  // shouldn't get here
  xmlUserFatalError("illegal attribute for a TypeVariable");
}

void XmlTypeReader::registerAttr_PseudoInstantiation
  (PseudoInstantiation *obj, int attr, char const *strValue) {
  // superclass
  if (registerAttr_NamedAtomicType_super(obj, attr, strValue)) return;

  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a PsuedoInstantiation"); break;
  case XTOK_primary: ul(primary, XTOK_CompoundType); break;
  case XTOK_args: ulEmbed(args, XTOK_List_PseudoInstantiation_args); break;
  }
}

void XmlTypeReader::registerAttr_DependentQType
  (DependentQType *obj, int attr, char const *strValue) {
  // superclass
  if (registerAttr_NamedAtomicType_super(obj, attr, strValue)) return;

  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a DependentQType"); break;
  case XTOK_first: ul(first, XTOK_AtomicType); break;
  case XTOK_rest: ul(rest, XTOK_PQName); break;
  }
}

bool XmlTypeReader::registerAttr_Scope_super(Scope *obj, int attr, char const *strValue) {
  switch(attr) {
  default: return false; break; // we didn't find it
  case XTOK_variables: ulEmbed(variables, XTOK_NameMap_Scope_variables); break;
  case XTOK_typeTags: ulEmbed(typeTags, XTOK_NameMap_Scope_typeTags); break;
  case XTOK_canAcceptNames: fromXml_bool(obj->canAcceptNames, strValue); break;
  case XTOK_parentScope: ul(parentScope, XTOK_Scope); break;
  case XTOK_scopeKind: fromXml(obj->scopeKind, strValue); break;
  case XTOK_namespaceVar: ul(namespaceVar, XTOK_Variable); break;
  case XTOK_templateParams: ulEmbed(templateParams, XTOK_List_Scope_templateParams); break;
  case XTOK_curCompound: ul(curCompound, XTOK_CompoundType); break;
  case XTOK_curLoc: fromXml_SourceLoc(obj->curLoc, strValue); break;
  }
  return true;                  // found it
}

void XmlTypeReader::registerAttr_Scope(Scope *obj, int attr, char const *strValue) {
  // "superclass": just re-use our own superclass code for ourself
  if (registerAttr_Scope_super(obj, attr, strValue)) return;
  // shouldn't get here
  xmlUserFatalError("illegal attribute for a Scope");
}

bool XmlTypeReader::registerAttr_BaseClass_super(BaseClass *obj, int attr, char const *strValue) {
  switch(attr) {
  default: return false; break;
  case XTOK_ct: ul(ct, XTOK_CompoundType); break;
  case XTOK_access: fromXml(obj->access, strValue); break;
  case XTOK_isVirtual: fromXml_bool(obj->isVirtual, strValue); break;
  }
  return true;
}

void XmlTypeReader::registerAttr_BaseClass(BaseClass *obj, int attr, char const *strValue) {
  // "superclass": just re-use our own superclass code for ourself
  if (registerAttr_BaseClass_super(obj, attr, strValue)) return;
  // shouldn't get here
  xmlUserFatalError("illegal attribute for a BaseClass");
}

void XmlTypeReader::registerAttr_BaseClassSubobj
  (BaseClassSubobj *obj, int attr, char const *strValue) {
  // "superclass": just re-use our own superclass code for ourself
  if (registerAttr_BaseClass_super(obj, attr, strValue)) return;

  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a BaseClassSubobj"); break;
  case XTOK_parents:
    // fprintf(stderr, "## registerAttr_BaseClassSubobj: id=%s, obj=%p, &obj->parents=%p\n",
    //         strValue, obj, &obj->parents);
    ulEmbed(parents, XTOK_List_BaseClassSubobj_parents); break;
  }
}

void XmlTypeReader::registerAttr_OverloadSet(OverloadSet *obj, int attr, char const *strValue) {
  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a OverloadSet"); break;
  case XTOK_set: ulEmbed(set, XTOK_List_OverloadSet_set); break;
  }
}

void XmlTypeReader::registerAttr_STemplateArgument
  (STemplateArgument *obj, int attr, char const *strValue) {
  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a STemplateArgument"); break;
  case XTOK_kind: fromXml(obj->kind, strValue); break;
  // exactly one of these must show up as it is a union; I don't check that though
  case XTOK_t: ul(value.t, XTOK_Type); break;
  case XTOK_i: fromXml_int(obj->value.i, strValue); break;
  case XTOK_v: ul(value.v, XTOK_Variable); break;
  case XTOK_e: ul(value.e, XTOK_Expression); break;
  case XTOK_at: ul(value.at, XTOK_AtomicType); break;
  }
}

bool XmlTypeReader::registerAttr_TemplateParams_super
  (TemplateParams *obj, int attr, char const *strValue) {
  switch(attr) {
  default: return false; break; // we didn't find it
  case XTOK_params: ulEmbed(params, XTOK_List_TemplateParams_params); break;
  }
  return true;
}

void XmlTypeReader::registerAttr_TemplateInfo(TemplateInfo *obj, int attr, char const *strValue) {
  // superclass
  if (registerAttr_TemplateParams_super(obj, attr, strValue)) return;

  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a TemplateInfo"); break;
  case XTOK_var: ul(var, XTOK_Variable); break;
  case XTOK_inheritedParams:
    ulEmbed(inheritedParams, XTOK_List_TemplateInfo_inheritedParams); break;
  case XTOK_instantiationOf:
    ul(instantiationOf, XTOK_Variable); break;
  case XTOK_instantiations:
    ulEmbed(instantiations, XTOK_List_TemplateInfo_instantiations); break;
  case XTOK_specializationOf:
    ul(specializationOf, XTOK_Variable); break;
  case XTOK_specializations:
    ulEmbed(specializations, XTOK_List_TemplateInfo_specializations); break;
  case XTOK_arguments:
    ulEmbed(arguments, XTOK_List_TemplateInfo_arguments); break;
  case XTOK_instLoc:
    fromXml_SourceLoc(obj->instLoc, strValue); break;
  case XTOK_partialInstantiationOf:
    ul(partialInstantiationOf, XTOK_Variable); break;
  case XTOK_partialInstantiations:
    ulEmbed(partialInstantiations, XTOK_List_TemplateInfo_partialInstantiations); break;
  case XTOK_argumentsToPrimary:
    ulEmbed(argumentsToPrimary, XTOK_List_TemplateInfo_argumentsToPrimary); break;
  case XTOK_defnScope:
    ul(defnScope, XTOK_Scope); break;
  case XTOK_definitionTemplateInfo:
    ul(definitionTemplateInfo, XTOK_TemplateInfo); break;
  }
}

void XmlTypeReader::registerAttr_InheritedTemplateParams
  (InheritedTemplateParams *obj, int attr, char const *strValue) {
  // superclass
  if (registerAttr_TemplateParams_super(obj, attr, strValue)) return;

  switch(attr) {
  default: xmlUserFatalError("illegal attribute for a InheritedTemplateParams"); break;
  case XTOK_enclosing:
    ul(enclosing, XTOK_CompoundType); break;
  }
}
