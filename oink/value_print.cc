// see License.txt for copyright and terms of use

#include "value_print.h"        // this module
#include "oink_var.h"           // asVariable_O

// **************** ValueTypePrinter

// **** **** copied from CTypePrinter

void ValueTypePrinter::print(OutStream &out, TypeLike const *valueOrType0, char const *name) {
  // temporarily suspend the Type::toCString, Variable::toCString(),
  // etc. methods
  Restorer<bool> res0(global_mayUseTypeAndVarToCString, false);
  TypeLike const *valueOrType = static_cast<TypeLike const*>(valueOrType0);
  if (valueOrType->isActuallyAValue()) {
    // VALUE
    // see the note at the interface TypePrinter::print()
    Value const *value0 = valueOrType->asValueC();
    out << print(value0, name);
  } else {
    // TYPE
    // this will fail unless the elsaTypePrinter has been reenabled
    elsaTypePrinter.print(out, valueOrType->asTypeC(), name);
  }
  // old way
//    out << type->toCString(name);
}


TypeLike const *ValueTypePrinter::getTypeLike(Variable const *var) {
  return asVariable_O(var)->abstrValue();
}


TypeLike const *ValueTypePrinter::getFunctionTypeLike(Function const *func) {
  // NOTE: Templatized functions do not have an abstract value so
  // sometimes we need to use the type instead.
  if (func->abstrValue) {
    return func->abstrValue;
  }
  // REMOVE_FUNC_DECL_VALUE: replace with this
//   if (asVariable_O(func->nameAndParams->var)->hasAbstrValue()) {
//     return asVariable_O(func->nameAndParams->var)->abstrValue();
//   }
  else {
    return func->funcType;
  }
}


TypeLike const *ValueTypePrinter::getE_constructorTypeLike(E_constructor const *c) {
  // NOTE: I templatizs we not have an abstract value so sometimes we
  // need to use the type instead.
  if (c->abstrValue) {
    return c->abstrValue;
  }
  else {
    return c->type;
  }
}


// called from cc_print.cc as a hook replacement
TypeLike const *getDeclarationRetTypeLike(TypeLike const *type) {
  // sm: The static_cast is superfluous, and I suspect appropriate
  // consts could be easily added to eliminate the need for the
  // const_cast too.

  // compute the retType/Value
  TypeLike *valueOrType = const_cast<TypeLike*>(static_cast<TypeLike const*>(type));
  if (valueOrType->isActuallyAValue()) {
    return valueOrType->asValue()->asFunctionValue()->retValue;
  } else {
    return valueOrType->asType()->asFunctionType()->retType;
  }
}

// called from cc_print.cc as a hook replacement
Type const *getDeclarationTypeLike(TypeLike const *type) {
  // compute the retType/Value
  TypeLike *valueOrType = const_cast<TypeLike*>(static_cast<TypeLike const*>(type));
  if (valueOrType->isActuallyAValue()) {
    return valueOrType->asValue()->t();
  } else {
    return valueOrType->asType();
  }
}


// **** AtomicType

string ValueTypePrinter::print(AtomicType const *atomic) {
  // roll our own virtual dispatch
  switch(atomic->getTag()) {
  default: xfailure("bad tag");
  case AtomicType::T_SIMPLE:              return print(atomic->asSimpleTypeC());
  case AtomicType::T_COMPOUND:            return print(atomic->asCompoundTypeC());
  case AtomicType::T_ENUM:                return print(atomic->asEnumTypeC());
  case AtomicType::T_TYPEVAR:             return print(atomic->asTypeVariableC());
  case AtomicType::T_PSEUDOINSTANTIATION: return print(atomic->asPseudoInstantiationC());
  case AtomicType::T_DEPENDENTQTYPE:      return print(atomic->asDependentQTypeC());
  }
}

string ValueTypePrinter::print(SimpleType const *simpleType) {
  return simpleTypeName(simpleType->type);
}

string ValueTypePrinter::print(CompoundType const *cpdType) {
  stringBuilder sb;

  TemplateInfo *tinfo = cpdType->templateInfo();
  bool hasParams = tinfo && tinfo->params.isNotEmpty();
  if (hasParams) {
    sb << tinfo->paramsToCString() << " ";
  }

  if (!tinfo || hasParams) {
    // only say 'class' if this is like a class definition, or
    // if we're not a template, since template instantiations
    // usually don't include the keyword 'class' (this isn't perfect..
    // I think I need more context)
    sb << CompoundType::keywordName(cpdType->keyword) << " ";
  }

  sb << (cpdType->instName? cpdType->instName : "/*anonymous*/");

  // template arguments are now in the name
  // 4/22/04: they were removed from the name a long time ago;
  //          but I'm just now uncommenting this code
  // 8/03/04: restored the original purpose of 'instName', so
  //          once again that is name+args, so this code is not needed
  //if (tinfo && tinfo->arguments.isNotEmpty()) {
  //  sb << sargsToString(tinfo->arguments);
  //}

  return sb;
}

string ValueTypePrinter::print(EnumType const *enumType) {
  return stringc << "enum " << (enumType->name? enumType->name : "/*anonymous*/");
}

string ValueTypePrinter::print(TypeVariable const *typeVar) {
  // use the "typename" syntax instead of "class", to distinguish
  // this from an ordinary class, and because it's syntax which
  // more properly suggests the ability to take on *any* type,
  // not just those of classes
  //
  // but, the 'typename' syntax can only be used in some specialized
  // circumstances.. so I'll suppress it in the general case and add
  // it explicitly when printing the few constructs that allow it
  //
  // 8/09/04: sm: truncated down to just the name, since the extra
  // clutter was annoying and not that helpful
  return stringc //<< "/""*typevar"
//                   << "typedefVar->serialNumber:"
//                   << (typedefVar ? typedefVar->serialNumber : -1)
                 //<< "*/"
                 << typeVar->name;
}

string ValueTypePrinter::print(PseudoInstantiation const *pseudoInst) {
  stringBuilder sb0;
  StringBuilderOutStream out0(sb0);
  CodeOutStream codeOut(out0);
  PrintEnv env(*this, &codeOut); // Yuck!
  // FIX: what about the env.loc?

  codeOut << pseudoInst->name;

  // NOTE: This was inlined from sargsToString; it would read as
  // follows:
//    codeOut << sargsToString(pseudoInst->args);
  codeOut << "<";
  int ct=0;
  FOREACH_OBJLIST(STemplateArgument, pseudoInst->args, iter) {
    if (ct++ > 0) {
      codeOut << ", ";
    }
    printSTemplateArgument(env, iter.data());
  }
  codeOut << ">";

  codeOut.finish();
  return sb0;
}

string ValueTypePrinter::print(DependentQType const *depType) {
  stringBuilder sb0;
  StringBuilderOutStream out0(sb0);
  CodeOutStream codeOut(out0);
  PrintEnv env(*this, &codeOut); // Yuck!
  // FIX: what about the env.loc?

  codeOut << print(depType->first) << "::";
  depType->rest->print(env);

  codeOut.finish();
  return sb0;
}


// **** [Compound]Value

string ValueTypePrinter::print(Value const *value) {
  // dsw: FIX: I think we don't want to do this because we might miss
  // something if we change in the general case but not in this
  // special case; maybe I'm worried about nothing
//    if (value->isCVAtomicValue()) {
//      // special case a single atomic type, so as to avoid
//      // printing an extra space
//      CVAtomicValue const *cvatomic = value->asCVAtomicValueC();
//      return stringc
//        << print(cvatomic->atomic)
//        << cvToString(cvatomic->cv);
//    }
//    else {
  return stringc
    << printLeft(value)
    << printRight(value);
//    }
}

string ValueTypePrinter::print(Value const *value, char const *name) {
  // print the inner parentheses if the name is omitted
  bool innerParen = (name && name[0])? false : true;

  #if 0    // wrong
  // except, if this value is a pointer, then omit the parens anyway;
  // we only need parens when the value is a function or array and
  // the name is missing
  if (isPointerValue()) {
    innerParen = false;
  }
  #endif // 0

  stringBuilder s;
  s << printLeft(value, innerParen);
  s << (name? name : "/*anon*/");
  s << printRight(value, innerParen);
  return s;
}

string ValueTypePrinter::printRight(Value const *value, bool innerParen) {
  // roll our own virtual dispatch
  switch(value->getTag()) {
  default: xfailure("illegal tag");
  case Value::V_ATOMIC:          return printRight(value->asCVAtomicValueC(), innerParen);
  case Value::V_POINTER:         return printRight(value->asPointerValueC(), innerParen);
  case Value::V_REFERENCE:       return printRight(value->asReferenceValueC(), innerParen);
  case Value::V_FUNCTION:        return printRight(value->asFunctionValueC(), innerParen);
  case Value::V_ARRAY:           return printRight(value->asArrayValueC(), innerParen);
  case Value::V_POINTERTOMEMBER: return printRight(value->asPointerToMemberValueC(), innerParen);
  }
}

string ValueTypePrinter::printLeft(Value const *value, bool innerParen) {
  // roll our own virtual dispatch
  switch(value->getTag()) {
  default: xfailure("illegal tag");
  case Value::V_ATOMIC:          return printLeft(value->asCVAtomicValueC(), innerParen);
  case Value::V_POINTER:         return printLeft(value->asPointerValueC(), innerParen);
  case Value::V_REFERENCE:       return printLeft(value->asReferenceValueC(), innerParen);
  case Value::V_FUNCTION:        return printLeft(value->asFunctionValueC(), innerParen);
  case Value::V_ARRAY:           return printLeft(value->asArrayValueC(), innerParen);
  case Value::V_POINTERTOMEMBER: return printLeft(value->asPointerToMemberValueC(), innerParen);
  }
}

string ValueTypePrinter::printLeft_orig(CVAtomicValue const *value, bool /*innerParen*/) {
  stringBuilder s;
  s << print(value->type->atomic);
  s << cvToString(value->type->cv);

  // this is the only mandatory space in the entire syntax
  // for declarations; it separates the value specifier from
  // the declarator(s)
  s << " ";

  return s;
}

string ValueTypePrinter::printRight(CVAtomicValue const *value, bool /*innerParen*/) {
  return "";
}

string ValueTypePrinter::printLeft_orig(PointerValue const *value, bool /*innerParen*/) {
  stringBuilder s;
  s << printLeft(value->atValue, false /*innerParen*/);
  if (value->atValue->isFunctionValue() ||
      value->atValue->isArrayValue()) {
    s << "(";
  }
  s << "*";
  if (value->type->cv) {
    // 1/03/03: added this space so "Foo * const arf" prints right (t0012.cc)
    s << cvToString(value->type->cv) << " ";
  }
  return s;
}

string ValueTypePrinter::printRight(PointerValue const *value, bool /*innerParen*/) {
  stringBuilder s;
  if (value->atValue->isFunctionValue() ||
      value->atValue->isArrayValue()) {
    s << ")";
  }
  s << printRight(value->atValue, false /*innerParen*/);
  return s;
}

string ValueTypePrinter::printLeft_orig(ReferenceValue const *value, bool /*innerParen*/) {
  stringBuilder s;
  s << printLeft(value->atValue, false /*innerParen*/);
  if (value->atValue->isFunctionValue() ||
      value->atValue->isArrayValue()) {
    s << "(";
  }
  s << "&";
  return s;
}

string ValueTypePrinter::printRight(ReferenceValue const *value, bool /*innerParen*/) {
  stringBuilder s;
  if (value->atValue->isFunctionValue() ||
      value->atValue->isArrayValue()) {
    s << ")";
  }
  s << printRight(value->atValue, false /*innerParen*/);
  return s;
}

string ValueTypePrinter::printLeft(FunctionValue const *value, bool innerParen) {
  stringBuilder sb;

  // FIX: FUNC TEMPLATE LOSS
  // template parameters
//    if (templateInfo) {
//      sb << templateInfo->paramsToCString() << " ";
//    }

  // return value and start of enclosing value's description
  if (value->type->flags & (/*FF_CONVERSION |*/ FF_CTOR | FF_DTOR)) {
    // don't print the return value, it's implicit

    // 7/18/03: changed so we print ret value for FF_CONVERSION,
    // since otherwise I can't tell what it converts to!
  }
  else {
    sb << printLeft(value->retValue);
  }

  // NOTE: we do *not* propagate 'innerParen'!
  if (innerParen) {
    sb << "(";
  }

  return sb;
}

string ValueTypePrinter::printRight(FunctionValue const *value, bool innerParen) {
  // I split this into two pieces because the Cqual++ concrete
  // syntax puts $tainted into the middle of my rightString,
  // since it's following the placement of 'const' and 'volatile'
  return stringc
    << printRightUpToQualifiers(value, innerParen)
    << printRightQualifiers(value, value->type->getReceiverCV())
    << printRightAfterQualifiers(value);
}

string ValueTypePrinter::printRightUpToQualifiers_orig(FunctionValue const *value, bool innerParen) {
  // finish enclosing value
  stringBuilder sb;
  if (innerParen) {
    sb << ")";
  }

  // arguments
  sb << "(";
  int ct=0;
  SFOREACH_OBJLIST(Variable_O, *value->params, iter) {
    ct++;
    if (value->type->isMethod() && ct==1) {
      // don't actually print the first parameter;
      // the 'm' stands for nonstatic member function
      sb << "/""*m: " << print(asVariable_O(iter.data())->abstrValue()) << " *""/ ";
      continue;
    }
    if (ct >= 3 || (!value->type->isMethod() && ct>=2)) {
      sb << ", ";
    }
    sb << printAsParameter(iter.data());
  }

  if (value->type->acceptsVarargs()) {
    if (ct++ > 0) {
      sb << ", ";
    }
    sb << "...";
  }

  sb << ")";

  return sb;
}

string ValueTypePrinter::printRightQualifiers(FunctionValue const *value, CVFlags cv) {
  if (cv) {
    return stringc << " " << ::toString(cv);
  }
  else {
    return "";
  }
}

string ValueTypePrinter::printRightAfterQualifiers(FunctionValue const *value) {
  stringBuilder sb0;
  {
    StringBuilderOutStream sbOut(sb0);

    // exception specs
    if (value->type->exnSpec) {
      sbOut << " throw(";
      int ct=0;
      // FIX: I suppose I need a value version of this
      SFOREACH_OBJLIST(Type, value->type->exnSpec->types, iter) {
        if (ct++ > 0) {
          sbOut << ", ";
        }
        Restorer<bool> res0(CTypePrinter::enabled, true);
        elsaTypePrinter.print(sbOut, const_cast<Type*>(iter.data()));
      }
      sbOut << ")";
    }
    sbOut.flush();
  }

  // hook for verifier syntax
  printExtraRightmostSyntax(value, sb0);

  // finish up the return value
  sb0 << printRight(value->retValue);

  return sb0;
}

void ValueTypePrinter::printExtraRightmostSyntax(FunctionValue const *value, stringBuilder &)
{}

string ValueTypePrinter::printLeft(ArrayValue const *value, bool /*innerParen*/) {
  return printLeft(value->atValue);
}

string ValueTypePrinter::printRight(ArrayValue const *value, bool /*innerParen*/) {
  stringBuilder sb;

  if (value->type->hasSize()) {
    sb << "[" << value->type->size << "]";
  }
  else {
    sb << "[]";
  }

  sb << printRight(value->atValue);

  return sb;
}

string ValueTypePrinter::printLeft(PointerToMemberValue const *value, bool /*innerParen*/) {
  stringBuilder s;
  s << printLeft(value->atValue, false /*innerParen*/);
  s << " ";
  if (value->atValue->isFunctionValue() ||
      value->atValue->isArrayValue()) {
    s << "(";
  }
  s << value->type->inClassNAT->name << "::*";
  s << cvToString(value->type->cv);
  return s;
}

string ValueTypePrinter::printRight(PointerToMemberValue const *value, bool /*innerParen*/) {
  stringBuilder s;
  if (value->atValue->isFunctionValue() ||
      value->atValue->isArrayValue()) {
    s << ")";
  }
  s << printRight(value->atValue, false /*innerParen*/);
  return s;
}

string ValueTypePrinter::printAsParameter(Variable const *var) {
  stringBuilder sb;
  if (var->type->isTypeVariable()) {
    // value variable's name, then the parameter's name (if any)
    sb << var->type->asTypeVariable()->name;
    if (var->name) {
      sb << " " << var->name;
    }
  }
  else {
    sb << print(asVariable_O(var)->abstrValue(), var->name);
  }

  if (var->value) {
    sb << renderExpressionAsString(" = ", var->value);
  }
  return sb;
}

// **** **** modified for oink

string ValueTypePrinter::printLeft(CVAtomicValue const *value, bool innerParen) {
  // FIX: sm: we can come back and optimize the placement of spaces later
  stringBuilder sb;
  sb << printLeft_orig(value, innerParen);
  leftString_extra(const_cast<CVAtomicValue*>(value)->asCVAtomicValue(), sb);
  return sb;
}

string ValueTypePrinter::printLeft(PointerValue const *value, bool innerParen) {
  stringBuilder sb;
  sb << printLeft_orig(value, innerParen);
  leftString_extra(const_cast<PointerValue*>(value)->asPointerValue(), sb);
  return sb;
}

string ValueTypePrinter::printLeft(ReferenceValue const *value, bool innerParen) {
  stringBuilder sb;
  sb << printLeft_orig(value, innerParen);
  leftString_extra(const_cast<ReferenceValue*>(value)->asReferenceValue(), sb);
  return sb;
}

string ValueTypePrinter::printRightUpToQualifiers(FunctionValue const *value, bool innerParen) {
  stringBuilder sb;
  sb << printRightUpToQualifiers_orig(value, innerParen);
  rightStringUpToQualifiers_extra(const_cast<FunctionValue*>(value)->asFunctionValue(), sb);
  return sb;
}


// **************** AST print() methods

void TS_typeVariable::print(PrintEnv &env) {
  TreeWalkDebug treeDebug("TS_typeVariable");
  env.loc = loc;
  *env.out << name;
}

void S_assert_type::iprint(PrintEnv &env) {
  TreeWalkDebug treeDebug("S_assert_type");
  env.loc = loc;
  *env.out << "__qual_assert_type";
  PairDelim pair(*env.out, "", "(", ")");
  e->print(env);
  *env.out << ",";
  tid->print(env);
}

void S_change_type::iprint(PrintEnv &env) {
  TreeWalkDebug treeDebug("S_change_type");
  env.loc = loc;
  *env.out << "__qual_change_type";
  PairDelim pair(*env.out, "", "(", ")");
  e->print(env);
  *env.out << ",";
  tid->print(env);
}
