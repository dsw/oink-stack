// xml_type_writer.cc            see license.txt for copyright and terms of use

#include "xml_type_writer.h"    // this module
#include "variable.h"           // Variable
#include "cc_flags.h"           // toXml(DeclFlags &out, rostring str)
#include "asthelp.h"            // xmlPrintPointer
#include "xmlhelp.h"            // toXml_int() etc.
#include "strutil.h"            // DelimStr
#include "cc_ast.h"             // AST nodes only for AST sub-traversals

#define serializeOracle serializeOracle_m

// toXml for enums

char const *toXml(CompoundType::Keyword id) {
  switch(id) {
  default: xfailure("bad enum"); break;
    PRINTENUM(CompoundType::K_STRUCT);
    PRINTENUM(CompoundType::K_CLASS);
    PRINTENUM(CompoundType::K_UNION);
  }
}

string toXml(FunctionFlags id) {
  if (id == FF_NONE) return "FF_NONE";
  DelimStr b('|');
  PRINTFLAG(FF_METHOD);
  PRINTFLAG(FF_VARARGS);
  PRINTFLAG(FF_CONVERSION);
  PRINTFLAG(FF_CTOR);
  PRINTFLAG(FF_DTOR);
  PRINTFLAG(FF_BUILTINOP);
  PRINTFLAG(FF_NO_PARAM_INFO);
  PRINTFLAG(FF_DEFAULT_ALLOC);
  PRINTFLAG(FF_KANDR_DEFN);
  return b.sb;
}

char const *toXml(ScopeKind id) {
  switch(id) {
  default: xfailure("bad enum"); break;
  PRINTENUM(SK_UNKNOWN);
  PRINTENUM(SK_GLOBAL);
  PRINTENUM(SK_PARAMETER);
  PRINTENUM(SK_FUNCTION);
  PRINTENUM(SK_CLASS);
  PRINTENUM(SK_TEMPLATE_PARAMS);
  PRINTENUM(SK_TEMPLATE_ARGS);
  PRINTENUM(SK_NAMESPACE);
  }
}

char const *toXml(STemplateArgument::Kind id) {
  switch(id) {
  default: xfailure("bad enum"); break;
  PRINTENUM(STemplateArgument::STA_NONE);
  PRINTENUM(STemplateArgument::STA_TYPE);
  PRINTENUM(STemplateArgument::STA_INT);
  PRINTENUM(STemplateArgument::STA_ENUMERATOR);
  PRINTENUM(STemplateArgument::STA_REFERENCE);
  PRINTENUM(STemplateArgument::STA_POINTER);
  PRINTENUM(STemplateArgument::STA_MEMBER);
  PRINTENUM(STemplateArgument::STA_DEPEXPR);
  PRINTENUM(STemplateArgument::STA_TEMPLATE);
  PRINTENUM(STemplateArgument::STA_ATOMIC);
  }
}

XmlTypeWriter::XmlTypeWriter (IdentityManager &idmgr0,
                              ASTVisitor *astVisitor0, ostream *out0, int &depth0, bool indent0,
                              XTW_SerializeOracle *serializeOracle0)
  : XmlWriter(idmgr0, out0, depth0, indent0)
  , astVisitor(astVisitor0)
  , serializeOracle(serializeOracle0)
{}

// **** toXml

// This one occurs in the AST, so it has to have its own first-class
// method.  FIX: This should not have such a general API and yet refer
// to such a specific list: PseudoInstantiation::args.
void XmlTypeWriter::toXml(ObjList<STemplateArgument> *list) {
  travObjList_standalone(*list, PseudoInstantiation, args, STemplateArgument);
}

void XmlTypeWriter::toXml(Type *t) {
  // idempotency
  if (idmgr.printed(t)) return;

  switch(t->getTag()) {
  default: xfailure("illegal tag");

  case Type::T_ATOMIC: {
    CVAtomicType *atom = t->asCVAtomicType();
    XmlTagPrinter tagPrinter(*this);
    if (writingP()) {
      tagPrinter.printOpenTag("CVAtomicType", atom);
      // **** attributes
      printPtr(atom, atomic);
      printXml(cv, atom->cv);
      tagPrinter.tagEnd();
    }
    // **** subtags
    trav(atom->atomic);
    break;
  }

  case Type::T_POINTER: {
    PointerType *ptr = t->asPointerType();
    XmlTagPrinter tagPrinter(*this);
    if (writingP()) {
      tagPrinter.printOpenTag("PointerType", ptr);
      // **** attributes
      printXml(cv, ptr->cv);
      printPtr(ptr, atType);
      tagPrinter.tagEnd();
    }

    // **** subtags
    trav(ptr->atType);
    break;
  }

  case Type::T_REFERENCE: {
    ReferenceType *ref = t->asReferenceType();
    XmlTagPrinter tagPrinter(*this);
    if (writingP()) {
      tagPrinter.printOpenTag("ReferenceType", ref);
      // **** attributes
      printPtr(ref, atType);
      tagPrinter.tagEnd();
    }

    // **** subtags
    trav(ref->atType);
    break;
  }

  case Type::T_FUNCTION: {
    FunctionType *func = t->asFunctionType();
    XmlTagPrinter tagPrinter(*this);
    if (writingP()) {
      tagPrinter.printOpenTag("FunctionType", func);
      // **** attributes
      printXml(flags, func->flags);
      printPtr(func, retType);
      printEmbed(func, params);
      printPtr(func, exnSpec);
      tagPrinter.tagEnd();
    }

    // **** subtags
    trav(func->retType);
    travObjList_S(func, FunctionType, params, Variable); // Variable
    // exnSpec
    if (func->exnSpec) {
      toXml_FunctionType_ExnSpec(func->exnSpec);
    }
    break;
  }

  case Type::T_ARRAY: {
    ArrayType *arr = t->asArrayType();
    XmlTagPrinter tagPrinter(*this);
    if (writingP()) {
      tagPrinter.printOpenTag("ArrayType", arr);
      // **** attributes
      printPtr(arr, eltType);
      printXml_int(size, arr->size);
      tagPrinter.tagEnd();
    }

    // **** subtags
    trav(arr->eltType);
    break;
  }

  case Type::T_POINTERTOMEMBER: {
    PointerToMemberType *ptm = t->asPointerToMemberType();
    XmlTagPrinter tagPrinter(*this);
    if (writingP()) {
      tagPrinter.printOpenTag("PointerToMemberType", ptm);
      // **** attributes
      printPtr(ptm, inClassNAT);
      printXml(cv, ptm->cv);
      printPtr(ptm, atType);
      tagPrinter.tagEnd();
    }

    // **** subtags
    trav(ptm->inClassNAT);
    trav(ptm->atType);
    break;
  }
  
  case Type::T_DEPENDENTSIZEDARRAY: {
    DependentSizedArrayType *dsa = t->asDependentSizedArrayType();
    XmlTagPrinter tagPrinter(*this);
    if (writingP()) {
      tagPrinter.printOpenTag("DependentSizedArrayType", dsa);
      // **** attributes
      printPtr(dsa, eltType);
      printPtrAST(dsa, sizeExpr);
      tagPrinter.tagEnd();
    }

    // **** subtags
    trav(dsa->eltType);
    break;
  }

  }
}

void XmlTypeWriter::toXml(AtomicType *atom) {
  // idempotency done in each sub-type as it is not done for
  // CompoundType here.
  switch(atom->getTag()) {
  default: xfailure("illegal tag");

  case AtomicType::T_SIMPLE: {
    // idempotency
    if (idmgr.printed(atom)) return;
    SimpleType *simple = atom->asSimpleType();
    XmlTagPrinter tagPrinter(*this);
    if (writingP()) {
      tagPrinter.printOpenTag("SimpleType", simple);
      // **** attributes
      printXml(type, simple->type);
      tagPrinter.tagEnd();
    }
    break;
  }

  case AtomicType::T_COMPOUND: {
    // NO!  Do NOT do this here:
//      // idempotency
//      if (idmgr.printed(atom)) return;
    CompoundType *cpd = atom->asCompoundType();
    toXml(cpd);
    break;
  }

  case AtomicType::T_ENUM: {
    // idempotency
    if (idmgr.printed(atom)) return;
    EnumType *e = atom->asEnumType();
    XmlTagPrinter tagPrinter(*this);
    if (writingP()) {
      tagPrinter.printOpenTag("EnumType", e);
      // **** attributes
      // * superclasses
      toXml_NamedAtomicType_properties(e);
      // * members
      printEmbed(e, valueIndex);
      printXml_int(nextValue, e->nextValue);
      printXml_bool(hasNegativeValues, e->hasNegativeValues);
      tagPrinter.tagEnd();
    }

    // **** subtags
    // * superclasses
    toXml_NamedAtomicType_subtags(e);
    // * members
    // valueIndex
    if (!idmgr.printed(&e->valueIndex)) {
      XmlTagPrinter tagPrinter2(*this);
      if (writingP()) {
        tagPrinter2.printOpenTag("NameMap_EnumType_valueIndex", &e->valueIndex);
        tagPrinter2.tagEnd();
      }

      if (sortNameMapDomainWhenSerializing) {
        for(StringObjDict<EnumType::Value>::SortedKeyIter iter(e->valueIndex);
            !iter.isDone(); iter.next()) {
          char const *name = iter.key();
          // dsw: do you know how bad it gets if I don't put a
          // const-cast here?
          EnumType::Value *eValue = const_cast<EnumType::Value*>(iter.value());

          XmlTagPrinter tagPrinter3(*this);
          if (writingP()) {
            tagPrinter3.printNameMapItemOpenTag(name, eValue);
          }

          toXml_EnumType_Value(eValue);
        }
      } else {
        for(StringObjDict<EnumType::Value>::Iter iter(e->valueIndex);
            !iter.isDone(); iter.next()) {
          rostring name = iter.key();
          // dsw: do you know how bad it gets if I don't put a
          // const-cast here?
          EnumType::Value *eValue = const_cast<EnumType::Value*>(iter.value());

          XmlTagPrinter tagPrinter3(*this);
          if (writingP()) {
            tagPrinter3.printNameMapItemOpenTag(name.c_str(), eValue);
          }

          toXml_EnumType_Value(eValue);
        }
      }
    }
    break;
  }

  case AtomicType::T_TYPEVAR: {
    // idempotency
    if (idmgr.printed(atom)) return;
    TypeVariable *tvar = atom->asTypeVariable();
    XmlTagPrinter tagPrinter(*this);
    if (writingP()) {
      tagPrinter.printOpenTag("TypeVariable", tvar);
      // **** attributes
      // * superclasses
      toXml_NamedAtomicType_properties(tvar);
      tagPrinter.tagEnd();
    }

    // **** subtags
    // * superclasses
    toXml_NamedAtomicType_subtags(tvar);
    break;
  }

  case AtomicType::T_PSEUDOINSTANTIATION: {
    // idempotency
    if (idmgr.printed(atom)) return;
    PseudoInstantiation *pseudo = atom->asPseudoInstantiation();
    XmlTagPrinter tagPrinter(*this);
    if (writingP()) {
      tagPrinter.printOpenTag("PseudoInstantiation", pseudo);
      // **** attributes
      // * superclasses
      toXml_NamedAtomicType_properties(pseudo);
      // * members
      printPtr(pseudo, primary);
      printEmbed(pseudo, args);
      tagPrinter.tagEnd();
    }

    // **** subtags
    // * superclasses
    toXml_NamedAtomicType_subtags(pseudo);
    // * members
    trav(pseudo->primary);
    travObjList(pseudo, PseudoInstantiation, args, STemplateArgument);
    break;
  }

  case AtomicType::T_DEPENDENTQTYPE: {
    // idempotency
    if (idmgr.printed(atom)) return;
    DependentQType *dep = atom->asDependentQType();
    XmlTagPrinter tagPrinter(*this);
    if (writingP()) {
      tagPrinter.printOpenTag("DependentQType", dep);
      // **** attributes
      // * superclasses
      toXml_NamedAtomicType_properties(dep);
      // * members
      printPtr(dep, first);
      printPtrAST(dep, rest);
      tagPrinter.tagEnd();
    }

    // **** subtags
    // * superclasses
    toXml_NamedAtomicType_subtags(dep);
    // * members
    trav(dep->first);
    travAST(dep->rest);
    break;
  }

  }
}

void XmlTypeWriter::toXml(CompoundType *cpd) {
  // idempotency
  if (idmgr.printed(cpd)) return;
  XmlTagPrinter tagPrinter(*this);
  if (writingP()) {
    tagPrinter.printOpenTag("CompoundType", cpd);
    // **** attributes
    // * superclasses
    toXml_NamedAtomicType_properties(cpd);
    toXml_Scope_properties(cpd);
    // * members
    printXml_bool(forward, cpd->forward);
    // FIX: isTransparentUnion; turn this on
//     printXml_bool(isTransparentUnion, cpd->isTransparentUnion);
    printXml(keyword, cpd->keyword);
    printEmbed(cpd, dataMembers);
    printEmbed(cpd, bases);
    printEmbed(cpd, virtualBases);
    printEmbed(cpd, subobj);
    printEmbed(cpd, conversionOperators);
    // FIX: friends; turn this on
//     printEmbed(cpd, friends);
    printStrRef(instName, cpd->instName);
    printPtrAST(cpd, syntax);
    printPtr(cpd, parameterizingScope);
    printPtr(cpd, selfType);
    tagPrinter.tagEnd();
  }

  // **** subtags
  // * superclasses
  toXml_NamedAtomicType_subtags(cpd);
  toXml_Scope_subtags(cpd);
  // * members
  travObjList_S(cpd, CompoundType, dataMembers, Variable); // Variable
  travObjList(cpd, CompoundType, bases, BaseClass);
  travObjList(cpd, CompoundType, virtualBases, BaseClassSubobj);
  toXml(&cpd->subobj);          // embedded
  travObjList_S(cpd, CompoundType, conversionOperators, Variable); // Variable
  // FIX: friends; turn this on
//   travObjList_S(cpd, CompoundType, friends, Variable); // Variable
  travAST(cpd->syntax);
  trav(cpd->parameterizingScope);
  trav(cpd->selfType);
}

void XmlTypeWriter::toXml_Variable_properties(Variable *var) {
  xassert(writingP());
  printXml_SourceLoc(loc, var->loc);
  printStrRef(name, var->name);
  printPtr(var, type);
  printXml(flags, var->flags);
  printPtrAST(var, value);
  printPtr(var, defaultParamType);
  printPtrAST(var, funcDefn);
  printPtr(var, overload);
  printPtr(var, scope);

  // **** these fields are abstractions; however here we pretend they
  // are real
  AccessKeyword access = var->getAccess();
  printXml(access, access);
  bool real = var->getReal();
  printXml_bool(real, real);
  bool maybeUsedAsAlias = var->getMaybeUsedAsAlias();
  printXml_bool(maybeUsedAsAlias, maybeUsedAsAlias);

  // dsw: perhaps these should instead be serialized by the client
  // code as they would have more meaningful names
  bool user1 = var->getUser1();
  printXml_bool(user1, user1);
  bool user2 = var->getUser2();
  printXml_bool(user2, user2);

  ScopeKind scopeKind = var->getScopeKind();
  printXml(scopeKind, scopeKind);
  bool hasValue = var->getHasValue();
  printXml_bool(hasValue, hasValue);
  int parameterOrdinal = var->getParameterOrdinal();
  printXml_int(parameterOrdinal, parameterOrdinal);
  // **** end abstract fields

  // dsw: FIX: usingAlias_or_parameterizedEntity is actually an
  // implicit union of two fields and should be serialized that way
  printPtr(var, usingAlias_or_parameterizedEntity);
  printPtr(var, templInfo);

  if (var->linkerVisibleName()) {
    newline();
    *out << "fullyQualifiedMangledName=";
    outputXmlAttrQuoted(*out, var->fullyQualifiedMangledName0());
  }
}

void XmlTypeWriter::toXml_Variable_subtags(Variable *var) {
  trav(var->type);
  travAST(var->value);
  trav(var->defaultParamType);
  travAST(var->funcDefn);
  trav(var->overload);
  trav(var->scope);
  trav(var->usingAlias_or_parameterizedEntity); // Variable
  trav(var->templInfo);
}

void XmlTypeWriter::toXml(Variable *var) {
#ifdef OINK
  xfailure("this should never be called in Oink");
#endif

  // idempotency
  if (idmgr.printed(var)) return;
  XmlTagPrinter tagPrinter(*this);
  if (writingP()) {
    tagPrinter.printOpenTag("Variable", var);
    // **** attributes
    toXml_Variable_properties(var);
    tagPrinter.tagEnd();
  }

  // **** subtags
  toXml_Variable_subtags(var);
}

void XmlTypeWriter::toXml_FunctionType_ExnSpec(void /*FunctionType::ExnSpec*/ *exnSpec0) {
  FunctionType::ExnSpec *exnSpec = static_cast<FunctionType::ExnSpec *>(exnSpec0);
  // idempotency
  if (idmgr.printed(exnSpec)) return;
  XmlTagPrinter tagPrinter(*this);
  if (writingP()) {
    tagPrinter.printOpenTag("FunctionType_ExnSpec", exnSpec);
    // **** attributes
    printEmbed(exnSpec, types);
    tagPrinter.tagEnd();
  }

  // **** subtags
  travObjList_S(exnSpec, ExnSpec, types, Type);
}

void XmlTypeWriter::toXml_EnumType_Value(void /*EnumType::Value*/ *eValue0) {
  EnumType::Value *eValue = static_cast<EnumType::Value *>(eValue0);
  // idempotency
  if (idmgr.printed(eValue)) return;
  XmlTagPrinter tagPrinter(*this);
  if (writingP()) {
    tagPrinter.printOpenTag("EnumType_Value", eValue);
    // **** attributes
    printStrRef(name, eValue->name);
    printPtr(eValue, type);
    printXml_int(value, eValue->value);

    // quarl 2006-05-31
    //    'decl' might be pointing at a Variable A which we have linked with
    //    another variable B, but we are serializing variable B and not A.
    //    See 'make oink-check-srz-multi-file-enum', which fails if we just
    //    serialize 'decl' directly.
    //
    //    One solution would be to add a union-find to variables so that we
    //    can follow and find the linked target. TODO.
    //
    //    For now we simply lose the 'decl' in such situations.
    if (!serializeOracle || serializeOracle->shouldSerialize(eValue->decl)) {
      printPtr(eValue, decl);
    }

    tagPrinter.tagEnd();
  }

  // **** subtags
  trav(eValue->type);
  trav(eValue->decl);           // Variable
}

void XmlTypeWriter::toXml_NamedAtomicType_properties(NamedAtomicType *nat) {
  printStrRef(name, nat->name);
  printPtr(nat, typedefVar);
  printXml(access, nat->access);
}

void XmlTypeWriter::toXml_NamedAtomicType_subtags(NamedAtomicType *nat) {
  trav(nat->typedefVar);        // Variable
}

void XmlTypeWriter::toXml(OverloadSet *oload) {
  // idempotency
  if (idmgr.printed(oload)) return;
  XmlTagPrinter tagPrinter(*this);
  if (writingP()) {
    tagPrinter.printOpenTag("OverloadSet", oload);
    // **** attributes
    printEmbed(oload, set);
    tagPrinter.tagEnd();
  }

  // **** subtags
  travObjList_S(oload, OverloadSet, set, Variable); // Variable
}

void XmlTypeWriter::toXml(BaseClass *bc) {
  // Since BaseClass objects are never manipulated polymorphically,
  // that is, every BaseClass pointer's static type equals its dynamic
  // type, 'bc' cannot actually be a BaseClassSubobj.

  // idempotency
  if (idmgr.printed(bc)) return;
  XmlTagPrinter tagPrinter(*this);
  if (writingP()) {
    tagPrinter.printOpenTag("BaseClass", bc);
    // **** attributes
    toXml_BaseClass_properties(bc);
    tagPrinter.tagEnd();
  }

  // **** subtags
  toXml_BaseClass_subtags(bc);
}

void XmlTypeWriter::toXml_BaseClass_properties(BaseClass *bc) {
  printPtr(bc, ct);
  printXml(access, bc->access);
  printXml_bool(isVirtual, bc->isVirtual);
}

void XmlTypeWriter::toXml_BaseClass_subtags(BaseClass *bc) {
  trav(bc->ct);
}

void XmlTypeWriter::toXml(BaseClassSubobj *bc) {
  // idempotency
  if (idmgr.printed(bc)) return;
  XmlTagPrinter tagPrinter(*this);
  if (writingP()) {
    tagPrinter.printOpenTag("BaseClassSubobj", bc);
    // **** attributes
    // * superclass
    toXml_BaseClass_properties(bc);
    // * members
    printEmbed(bc, parents);
    tagPrinter.tagEnd();
  }

  // **** subtags
  // * superclass
  toXml_BaseClass_subtags(bc);
  // * members
  travObjList_S(bc, BaseClassSubobj, parents, BaseClassSubobj);
}

void XmlTypeWriter::toXml(Scope *scope) {
  // are we really a CompoundType?
  if (CompoundType *cpd = dynamic_cast<CompoundType*>(scope)) {
    toXml(cpd);
    return;
  }
  // idempotency
  if (idmgr.printed(scope)) return;
  XmlTagPrinter tagPrinter(*this);
  if (writingP()) {
    tagPrinter.printOpenTag("Scope", scope);
    // **** attributes
    toXml_Scope_properties(scope);
    tagPrinter.tagEnd();
  }

  // **** subtags
  toXml_Scope_subtags(scope);
}

void XmlTypeWriter::toXml_Scope_properties(Scope *scope) {
  printEmbed(scope, variables);
  printEmbed(scope, typeTags);
  printXml_bool(canAcceptNames, scope->canAcceptNames);
  printPtr(scope, parentScope);
  printXml(scopeKind, scope->scopeKind);
  printPtr(scope, namespaceVar);
  printEmbed(scope, templateParams);
  printPtr(scope, curCompound);
  printXml_SourceLoc(curLoc, scope->curLoc);
}

void XmlTypeWriter::toXml_Scope_subtags(Scope *scope) {
  // FIX: these are like function template "partial specializations"
  // (if such things existed), as the 'Variable' paramter actually
  // also changes the implementation.
  travStringRefMap(scope, Scope, variables, Variable); // Variable
  travStringRefMap(scope, Scope, typeTags, Variable); // Variable
  trav(scope->parentScope);
  trav(scope->namespaceVar);    // Variable
  travObjList_S(scope, Scope, templateParams, Variable); // Variable
  trav(scope->curCompound);
}

void XmlTypeWriter::toXml(STemplateArgument *sta) {
  // idempotency
  if (idmgr.printed(sta)) return;
  XmlTagPrinter tagPrinter(*this);
  if (writingP()) {
    tagPrinter.printOpenTag("STemplateArgument", sta);

    // **** attributes
    printXml(kind, sta->kind);
    newline();

    switch(sta->kind) {
    default: xfailure("illegal STemplateArgument kind"); break;

    case STemplateArgument::STA_TYPE:
      printPtrUnion(sta, value.t, t);
      break;

    case STemplateArgument::STA_INT:
      printXml_int(i, sta->value.i);
      break;

    case STemplateArgument::STA_ENUMERATOR:
    case STemplateArgument::STA_REFERENCE:
    case STemplateArgument::STA_POINTER:
    case STemplateArgument::STA_MEMBER:
      printPtrUnion(sta, value.v, v);
      break;

    case STemplateArgument::STA_DEPEXPR:
      printPtrASTUnion(sta, value.e, e);
      break;

    case STemplateArgument::STA_TEMPLATE:
      xfailure("template template arguments not implemented");
      break;

    case STemplateArgument::STA_ATOMIC:
      printPtrUnion(sta, value.at, at);
      break;
    }
    tagPrinter.tagEnd();
  }

  // **** subtags

  // NOTE: I don't use the trav() macro here because it would be weird
  // to test the member of a union for being NULL; it should have a
  // well-defined value if it is the selected type of the tag.
  switch(sta->kind) {
  default: xfailure("illegal STemplateArgument kind"); break;
  case STemplateArgument::STA_TYPE:
    toXml(sta->value.t);
    break;

  case STemplateArgument::STA_INT:
    // nothing to do
    break;

  case STemplateArgument::STA_ENUMERATOR:
  case STemplateArgument::STA_REFERENCE:
  case STemplateArgument::STA_POINTER:
  case STemplateArgument::STA_MEMBER:
    trav(sta->value.v);         // Variable
    break;

  case STemplateArgument::STA_DEPEXPR:
    // (quarl) I don't get what's going on here; this looks weird.
    if (astVisitor) {
      // const_cast: traversal is sort of const-polymorphic...
      const_cast<Expression*>(sta->value.e)->traverse(*astVisitor);
    }
    break;

  case STemplateArgument::STA_TEMPLATE:
    xfailure("template template arguments not implemented");
    break;

  case STemplateArgument::STA_ATOMIC:
    toXml(const_cast<AtomicType*>(sta->value.at));
    break;
  }
}

void XmlTypeWriter::toXml(TemplateInfo *ti) {
  // idempotency
  if (idmgr.printed(ti)) return;
  XmlTagPrinter tagPrinter(*this);
  if (writingP()) {
    tagPrinter.printOpenTag("TemplateInfo", ti);
    // **** attributes
    // * superclass
    toXml_TemplateParams_properties(ti);
    // * members
    printPtr(ti, var);
    printEmbed(ti, inheritedParams);
    printPtr(ti, instantiationOf);
    printEmbed(ti, instantiations);
    printPtr(ti, specializationOf);
    printEmbed(ti, specializations);
    printEmbed(ti, arguments);
    printXml_SourceLoc(instLoc, ti->instLoc);
    printPtr(ti, partialInstantiationOf);
    printEmbed(ti, partialInstantiations);
    printEmbed(ti, argumentsToPrimary);
    printPtr(ti, defnScope);
    printPtr(ti, definitionTemplateInfo);
    tagPrinter.tagEnd();
  }

  // **** subtags
  // * superclass
  toXml_TemplateParams_subtags(ti);
  // * members
  trav(ti->var);                // Variable
  travObjList(ti, TemplateInfo, inheritedParams, InheritedTemplateParams);
  trav(ti->instantiationOf);    // Variable
  travObjList_S(ti, TemplateInfo, instantiations, Variable); // Variable
  trav(ti->specializationOf);   // Variable
  travObjList_S(ti, TemplateInfo, specializations, Variable); // Variable
  travObjList(ti, TemplateInfo, arguments, STemplateArgument);
  trav(ti->partialInstantiationOf); // Variable
  travObjList_S(ti, TemplateInfo, partialInstantiations, Variable); // Variable
  travObjList(ti, TemplateInfo, argumentsToPrimary, STemplateArgument);
  trav(ti->defnScope);
  trav(ti->definitionTemplateInfo);
}

void XmlTypeWriter::toXml(InheritedTemplateParams *itp) {
  // idempotency
  if (idmgr.printed(itp)) return;
  XmlTagPrinter tagPrinter(*this);
  if (writingP()) {
    tagPrinter.printOpenTag("InheritedTemplateParams", itp);
    // **** attributes
    // * superclass
    toXml_TemplateParams_properties(itp);
    // * members
    printPtr(itp, enclosing);
    tagPrinter.tagEnd();
  }

  // **** subtags
  // * superclass
  toXml_TemplateParams_subtags(itp);
  // * members
  trav(itp->enclosing);
}

void XmlTypeWriter::toXml_TemplateParams_properties(TemplateParams *tp) {
  printEmbed(tp, params);
}

void XmlTypeWriter::toXml_TemplateParams_subtags(TemplateParams *tp) {
  travObjList_S(tp, TemplateParams, params, Variable); // Variable
}


// **** shouldSerialize

bool XmlTypeWriter::XTW_SerializeOracle::shouldSerialize(AtomicType const *obj) {
  if (CompoundType const *cpd = dynamic_cast<CompoundType const *>(obj)) {
    return shouldSerialize(cpd);
  }
  return true;
}

bool XmlTypeWriter::XTW_SerializeOracle::shouldSerialize(Scope const *obj) {
  if (CompoundType const *cpd = dynamic_cast<CompoundType const *>(obj)) {
    return shouldSerialize(cpd);
  }
  return true;
}


// **** class XmlTypeWriter_AstVisitor

XmlTypeWriter_AstVisitor::XmlTypeWriter_AstVisitor
  (XmlTypeWriter &ttx0,
   ostream &out0,
   int &depth0,
   bool indent0)
    : XmlAstWriter_AstVisitor(out0, ttx0.idmgr, depth0, indent0)
    , ttx(ttx0)
{}

// Note that idempotency is handled in XmlTypeWriter
// #define PRINT_ANNOT(A) if (A) {ttx.toXml(A);}
#define PRINT_ANNOT_MAYBE(A) if (A && (!ttx.serializeOracle || ttx.serializeOracle->shouldSerialize(A))) {ttx.toXml(A);}

  // this was part of the macro
//    printASTBiLink((void**)&(A), (A));

  // print the link between the ast node and the annotating node
//    void printASTBiLink(void **astField, void *annotation) {
//      out << "<__Link from=\"";
//      // this is not from an ast *node* but from the *field* of one
//      xmlPrintPointer(out, "FLD", uniqueId(astField));
//      out << "\" to=\"";
//      xmlPrintPointer(out, "TY", uniqueId(annotation));
//      out << "\"/>\n";
//    }

bool XmlTypeWriter_AstVisitor::visitTranslationUnit(TranslationUnit *unit) {
  if (!XmlAstWriter_AstVisitor::visitTranslationUnit(unit)) return false;
  PRINT_ANNOT_MAYBE(unit->globalScope);
  return true;
}

bool XmlTypeWriter_AstVisitor::visitTypeSpecifier(TypeSpecifier *ts) {
  if (!XmlAstWriter_AstVisitor::visitTypeSpecifier(ts)) return false;
  if (ts->isTS_type()) {
    PRINT_ANNOT_MAYBE(ts->asTS_type()->type);
  } else if (ts->isTS_name()) {
    PRINT_ANNOT_MAYBE(ts->asTS_name()->var); // Variable
    PRINT_ANNOT_MAYBE(ts->asTS_name()->nondependentVar); // Variable
  } else if (ts->isTS_elaborated()) {
    PRINT_ANNOT_MAYBE(ts->asTS_elaborated()->atype);
  } else if (ts->isTS_classSpec()) {
    PRINT_ANNOT_MAYBE(ts->asTS_classSpec()->ctype);
  } else if (ts->isTS_enumSpec()) {
    PRINT_ANNOT_MAYBE(ts->asTS_enumSpec()->etype);
  }
  return true;
}

bool XmlTypeWriter_AstVisitor::visitFunction(Function *f) {
  if (!XmlAstWriter_AstVisitor::visitFunction(f)) return false;
  PRINT_ANNOT_MAYBE(f->funcType);
  PRINT_ANNOT_MAYBE(f->receiver); // Variable
  PRINT_ANNOT_MAYBE(f->retVar); // Variable
  return true;
}

bool XmlTypeWriter_AstVisitor::visitMemberInit(MemberInit *memberInit) {
  if (!XmlAstWriter_AstVisitor::visitMemberInit(memberInit)) return false;
  PRINT_ANNOT_MAYBE(memberInit->member); // Variable
  PRINT_ANNOT_MAYBE(memberInit->base);
  PRINT_ANNOT_MAYBE(memberInit->ctorVar); // Variable
  return true;
}

bool XmlTypeWriter_AstVisitor::visitBaseClassSpec(BaseClassSpec *bcs) {
  if (!XmlAstWriter_AstVisitor::visitBaseClassSpec(bcs)) return false;
  PRINT_ANNOT_MAYBE(bcs->type);
  return true;
}

bool XmlTypeWriter_AstVisitor::visitDeclarator(Declarator *d) {
  if (!XmlAstWriter_AstVisitor::visitDeclarator(d)) return false;
  PRINT_ANNOT_MAYBE(d->var);    // Variable
  PRINT_ANNOT_MAYBE(d->type);
  return true;
}

bool XmlTypeWriter_AstVisitor::visitExpression(Expression *e) {
  if (!XmlAstWriter_AstVisitor::visitExpression(e)) return false;
  PRINT_ANNOT_MAYBE(e->type);
  if (e->isE_this()) {
    PRINT_ANNOT_MAYBE(e->asE_this()->receiver); // Variable
  } else if (e->isE_variable()) {
    PRINT_ANNOT_MAYBE(e->asE_variable()->var); // Variable
    PRINT_ANNOT_MAYBE(e->asE_variable()->nondependentVar); // Variable
  } else if (e->isE_constructor()) {
    PRINT_ANNOT_MAYBE(e->asE_constructor()->ctorVar); // Variable
  } else if (e->isE_fieldAcc()) {
    PRINT_ANNOT_MAYBE(e->asE_fieldAcc()->field); // Variable
  } else if (e->isE_new()) {
    PRINT_ANNOT_MAYBE(e->asE_new()->ctorVar); // Variable
    PRINT_ANNOT_MAYBE(e->asE_new()->heapVar); // Variable
  } else if (e->isE_throw()) {
    PRINT_ANNOT_MAYBE(e->asE_throw()->globalVar); // Variable
  }
  return true;
}

#ifdef GNU_EXTENSION
bool XmlTypeWriter_AstVisitor::visitASTTypeof(ASTTypeof *a) {
  if (!XmlAstWriter_AstVisitor::visitASTTypeof(a)) return false;
  PRINT_ANNOT_MAYBE(a->type);
  return true;
}
#endif // GNU_EXTENSION

bool XmlTypeWriter_AstVisitor::visitPQName(PQName *pqn) {
  if (!XmlAstWriter_AstVisitor::visitPQName(pqn)) return false;
  if (pqn->isPQ_qualifier()) {
    PRINT_ANNOT_MAYBE(pqn->asPQ_qualifier()->qualifierVar); // Variable
    ttx.toXml(&(pqn->asPQ_qualifier()->sargs));
  } else if (pqn->isPQ_template()) {
    ttx.toXml(&(pqn->asPQ_template()->sargs));
  } else if (pqn->isPQ_variable()) {
    PRINT_ANNOT_MAYBE(pqn->asPQ_variable()->var); // Variable
  }
  return true;
}

bool XmlTypeWriter_AstVisitor::visitEnumerator(Enumerator *e) {
  if (!XmlAstWriter_AstVisitor::visitEnumerator(e)) return false;
  PRINT_ANNOT_MAYBE(e->var);    // Variable
  return true;
}

bool XmlTypeWriter_AstVisitor::visitInitializer(Initializer *e) {
  if (!XmlAstWriter_AstVisitor::visitInitializer(e)) return false;
  if (e->isIN_ctor()) {
    PRINT_ANNOT_MAYBE(e->asIN_ctor()->ctorVar); // Variable
  }
  return true;
}

bool XmlTypeWriter_AstVisitor::visitTemplateParameter(TemplateParameter *tparam) {
  if (!XmlAstWriter_AstVisitor::visitTemplateParameter(tparam)) return false;
  PRINT_ANNOT_MAYBE(tparam->var); // Variable
  return true;
}

bool XmlTypeWriter_AstVisitor::visitHandler(Handler *h) {
  if (!XmlAstWriter_AstVisitor::visitHandler(h)) return false;
  PRINT_ANNOT_MAYBE(h->globalVar); // Variable
  return true;
}

// #undef PRINT_ANNOT
#undef PRINT_ANNOT_MAYBE
