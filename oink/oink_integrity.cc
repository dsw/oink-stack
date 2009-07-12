// see License.txt for copyright and terms of use
// code for oink_integrity.h

#include "oink_integrity.h"     // this module
#include "oink_var.h"
#include "oink_global.h"        // oinkCmd
#include <stdio.h>              // printf

// **** AST visitor

bool IntegrityVisitor::visitFunction(Function *obj) {
  if (asVariable_O(obj->nameAndParams->var)->filteredOut()) return false;
  // REMOVE_FUNC_DECL_VALUE: remove this
  integrity_Constructed(obj->abstrValue);
  if (obj->retVar) integrity(obj->retVar); //
  if (obj->receiver) integrity(obj->receiver); //
  return true;
}

bool IntegrityVisitor::visitMemberInit(MemberInit *obj)
{
  if (obj->member) integrity(obj->member);        //
  if (obj->ctorVar) integrity(obj->ctorVar);       //
  return true;
}

bool IntegrityVisitor::visitPQName(PQName *obj)
{
  // SGM 2007-08-25: To correspond with the change I made to
  // RealVarAndTypeASTVisitor to not descend inside PQ_template, this
  // must also not look inside PQ_template.
  if (obj->isPQ_template()) {
    return false;
  }

  if (obj->isPQ_variable()) {
    integrity(obj->asPQ_variable()->var); //
  }
  return true;
}

bool IntegrityVisitor::visitTypeSpecifier(TypeSpecifier *spec)
{
  // FIX: this causes a failure if you turn it on
//    if (spec->isTS_name()) {
//      integrity(spec->asTS_name()->var);
//    } else if (spec->isTS_elaborated()) {
//      checkIntegrity_AtomicType(spec->asTS_elaborated()->atype);
//    } else if (spec->isTS_classSpec()) {
//      TS_classSpec *ts = spec->asTS_classSpec();
//      if (ts->ctype) {
//        integrity(ts->ctype->asCompoundType()->getTypedefVar());
//      }
//    } else if (spec->isTS_enumSpec()) {
//      checkIntegrity_AtomicType(spec->asTS_enumSpec()->etype);
//    }

  return true;
}

bool IntegrityVisitor::visitEnumerator(Enumerator *obj)
{
  integrity(obj->var);           //
  return true;
}

bool IntegrityVisitor::visitDeclarator(Declarator *obj)
{
  if (obj->context == DC_UNKNOWN) {
    printf("error: Declarator::context == DC_UNKNOWN\n");
    xfailure("integrity error");
  }

  // this declarator exists to help parsing/tcheck, oink/qual should
  // not be looking at it, so we'll skip it
  if (obj->context == DC_ON_CONVERSION) return true;

  if (asVariable_O(obj->var)->filteredOut()) return false;

  // obj->type and obj->var fail to be non-null at
  // test/template_class2_forward1.cc.filter-good.cc:10 at the name of
  // a template definition argument; FIX: is this a bug?
  //                                  v--- here
  //     template<class T2> struct A<T2*> {
  //
  // REMOVE_FUNC_DECL_VALUE: remove this 'if' clause
  if (obj->abstrValue) {
    integrity_Constructed(obj->abstrValue);         //
  }
  if (obj->var) {
    integrity(obj->var);           //
  }
  return true;
}

bool IntegrityVisitor::visitHandler(Handler *obj)
{
  if (obj->globalVar) integrity(obj->globalVar);
  return true;
}

bool IntegrityVisitor::visitExpression(Expression *obj)
{
  // FIX: E_stringLit::continuation does not get a type, so I have
  // to do this.
  if (!obj->isE_stringLit()) {
    if (oinkCmd->merge_E_variable_and_var_values && obj->isE_variable()) {
      return true;
      // we share this value with the variable; nothing to check
      // really.  The following check would have to be generalized to
      // deal with rvalue/lvalue mis-match if it is turned on.
//       xassert(asVariable_O(obj->asE_variable()->var)->hasAbstrValue());
//       xassert(obj->abstrValue == asVariable_O(obj->asE_variable()->var)->abstrValue());
    } else {
      integrity_Constructed(obj->abstrValue); //
    }
  }

  if (obj->isE_new()) {
    if (obj->asE_new()->heapVar) integrity(obj->asE_new()->heapVar); //
  } else if (obj->isE_throw()) {
    if (obj->asE_throw()->globalVar) integrity(obj->asE_throw()->globalVar); //
  } else if (obj->isE_variable()) {
    if (obj->asE_variable()->var) integrity(obj->asE_variable()->var); //
  } else if (obj->isE_constructor()) {
    if (obj->asE_constructor()->ctorVar) integrity(obj->asE_constructor()->ctorVar); //
  } else if (obj->isE_fieldAcc()) {
    if (obj->asE_fieldAcc()->field) integrity(obj->asE_fieldAcc()->field); //
  } else if (obj->isE_new()) {
    integrity(obj->asE_new()->ctorVar); //
  }
  return true;
}

bool IntegrityVisitor::visitInitializer(Initializer *obj)
{
  if (obj->isIN_ctor()) {
    if (obj->asIN_ctor()->ctorVar) integrity(obj->asIN_ctor()->ctorVar); //
  }
  return true;
}

bool IntegrityVisitor::visitTemplateDeclaration(TemplateDeclaration *obj)
{
  xfailure("IntegrityVisitor::visitTemplateDeclaration should never be called");
}

// gnu.ast
// NOTE: missing in cc_gdb.ast also
//  new class ASTTypeof {
//    public Type *type;

// **** check the integrity of the type tree

void IntegrityVisitor::checkIntegrity_AtomicType(AtomicType const *type)
{
  xassert(type);
  if (!checkAtomicTypes) return;
  if (atomicTypeSet.contains(type)) {
    // FIX: atomic types don't have serial numbers, but now one uses
    // this right now anyway.
//      printf("error: Atomic Type t%d reachable in two different ways\n",
//             t->serialNumber);
    xfailure("integrity error");
  }
  atomicTypeSet.add(type);
}

void IntegrityVisitor::checkIntegrity_ConstructedValue(Value const *value)
{
  xassert(value);
  xassert(checkConstructedValues);
  if (constructedValueSet.contains(value)) {
    #if USE_SERIAL_NUMBERS
    printf("error: Constructed Value v%d reachable in two different ways\n",
           value->serialNumber);
    #endif // USE_SERIAL_NUMBERS
    xfailure("integrity error (19097cc2-d116-4d6a-b218-7fac6acf45ce)");
  }
  constructedValueSet.add(value);
}

// Atomic
void IntegrityVisitor::integrity_Atomic(AtomicType const *type)
{
  switch(type->getTag()) {
  default: xfailure("illegal atomic type tag"); break;
  case AtomicType::T_SIMPLE:   integrity(static_cast<SimpleType const *>  (type)); break;
  case AtomicType::T_COMPOUND: integrity(static_cast<CompoundType const *>(type)); break;
  case AtomicType::T_ENUM:     integrity(static_cast<EnumType const *>    (type)); break;

  case AtomicType::T_TYPEVAR:
    // integrity(static_cast<TypeVariable const *>(type)); break;
    xfailure("Integrity failure: should not see T_TYPEVAR (0a00f982-9ebb-44e8-96cd-43b7db971fcb)"); break;

  case AtomicType::T_PSEUDOINSTANTIATION:
    // integrity(static_cast<PseudoInstantiation const *>(type)); break;
    xfailure("Integrity failure: should not see T_PSEUDOINSTANTIATION (7abfd88e-8898-477a-8312-e2cab6bfd0fe)"); break;

  case AtomicType::T_DEPENDENTQTYPE:
    xfailure("Integrity failure: should not see T_DEPENDENTQTYPE (1d5fad1c-35f6-4c6c-9262-f8b8a0a22875)"); break;
  }
}

void IntegrityVisitor::integrity(SimpleType const *type)
{
  checkIntegrity_AtomicType(type);
}

void IntegrityVisitor::integrity(CompoundType const *type)
{
  checkIntegrity_AtomicType(type);
}

void IntegrityVisitor::integrity(EnumType const *type)
{
  checkIntegrity_AtomicType(type);
}

void IntegrityVisitor::integrity(TypeVariable const *type)
{
  checkIntegrity_AtomicType(type);
}

// FIX: I haven't checked that this is right, I just copied it from
// the above.
void IntegrityVisitor::integrity(PseudoInstantiation const *type)
{
  checkIntegrity_AtomicType(type);
}

// Constructed
void IntegrityVisitor::integrity_Constructed(Value const *value)
{
  // NOTE: value may be NULL
  if (!checkConstructedValues) return;
  xassert(value);
  switch(value->getTag()) {
  default: xfailure("illegal constructed value tag"); break;
  case Value::V_ATOMIC:
    integrity(static_cast<CVAtomicValue const *>       (value)); break;
  case Value::V_POINTER:
    integrity(static_cast<PointerValue const *>        (value)); break;
  case Value::V_REFERENCE:
    integrity(static_cast<ReferenceValue const *>      (value)); break;
  case Value::V_FUNCTION:
    integrity(static_cast<FunctionValue const *>       (value)); break;
  case Value::V_ARRAY:
    integrity(static_cast<ArrayValue const *>          (value)); break;
  case Value::V_POINTERTOMEMBER:
    integrity(static_cast<PointerToMemberValue const *>(value)); break;
  }
}

void IntegrityVisitor::integrity(CVAtomicValue const *value)
{
  checkIntegrity_ConstructedValue(value);
//    putSerialNo(sb);
  integrity_Atomic(value->type->atomic);
}

void IntegrityVisitor::integrity(PointerValue const *value)
{
  checkIntegrity_ConstructedValue(value);
//    putSerialNo(sb);
  integrity_Constructed(value->atValue);
}

void IntegrityVisitor::integrity(ReferenceValue const *value)
{
  checkIntegrity_ConstructedValue(value);
//    putSerialNo(sb);
  integrity_Constructed(value->atValue);
}

void IntegrityVisitor::integrity(FunctionValue const *value)
{
  checkIntegrity_ConstructedValue(value);
//    if (templateParams) {
//      integrity_Constructed(templateParams);
//    }
//    putSerialNo(sb);
  integrity_Constructed(value->retValue);

  // arg, you can't do this due to the way Scott handles retVar
//    if (retVar) {
//      integrity_Constructed(retVar);
//    }

  SFOREACH_OBJLIST(Variable_O, *value->params, iter) {
    Variable_O const *v = asVariable_O(iter.data());
    integrity(v);
  }

  if (value->hasEllipsis()) {
    integrity(const_cast<FunctionValue*>(value)->getEllipsis());
  }

  if (value->retVar) {
    integrity(value->retVar);
  }
}

void IntegrityVisitor::integrity(ArrayValue const *value)
{
  checkIntegrity_ConstructedValue(value);
//    putSerialNo(sb);
  integrity_Constructed(value->atValue);
}

void IntegrityVisitor::integrity(PointerToMemberValue const *value)
{
  checkIntegrity_ConstructedValue(value);
//    putSerialNo(sb);
  integrity_Constructed(value->atValue);
}

// **** check integrity of variables

void IntegrityVisitor::integrity(Variable const *var)
{
  xassert(var);
  if (varSet.contains(var)) return; // this method is idempotent
  varSet.add(var);
  xassert(var->getReal());
  // NOTE: don't use Variable::getInstanceSpecificValue() here.  Don't
  // force the existance of an abstract value annotation if one
  // doesn't already exist; some passes may not ever generate an
  // abstract value annotation.
  if (asVariable_O(var)->hasAbstrValue()) {
    integrity_Constructed(asVariable_O(var)->abstrValue());
  }
}
