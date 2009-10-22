// see License.txt for copyright and terms of use

#include "qual_xml_value_writer.h" // this module
#include "qual_value_children.h"   // treeContainsExternQvars

#define serializeOracle (dynamic_cast<XmlValueWriter_Q::XVWQ_SerializeOracle*> (serializeOracle_m))

XmlValueWriter_Q::XmlValueWriter_Q
  (IdentityManager &mgr,
   // VarPredicateFunc *varPred0, ValuePredicateFunc *valuePred0,
   ASTVisitor *astVisitor0, ValueVisitor *valueVisitor0,
   std::ostream *out0, int &depth0, bool indent0,
   XVW_SerializeOracle *srzOracle0)
    : XmlValueWriter(mgr, /*varPred0, valuePred0,*/ astVisitor0, valueVisitor0, out0, depth0,
                     indent0, srzOracle0)
{}

char const * const XmlValueWriter_Q::tagName_Value(Value *v) {
  switch(v->getTag()) {
  default: xfailure("illegal tag");
  case Value::V_ATOMIC: return "CVAtomicValue_Q";
  case Value::V_POINTER: return "PointerValue_Q";
  case Value::V_REFERENCE: return "ReferenceValue_Q";
  case Value::V_FUNCTION: return "FunctionValue_Q";
  case Value::V_ARRAY: return "ArrayValue_Q";
  case Value::V_POINTERTOMEMBER: return "PointerToMemberValue_Q";
  }
}

char const * const XmlValueWriter_Q::tagName_Variable() {
  return "Variable_Q";
}

void XmlValueWriter_Q::toXml_externVars(TailList<Variable_O> *list0) {
  TailList<Variable_Q> *list = reinterpret_cast<TailList<Variable_Q>*>(list0);
  travObjList0(*list, externVars, Variable_Q, FOREACH_TAILLIST_NC, TailList);
}

bool XmlValueWriter_Q::XVWQ_SerializeOracle::shouldSerialize(Value const *value) {
  return value && treeContainsExternQvars(value); // && valuePred(value);
}

bool XmlValueWriter_Q::XVWQ_SerializeOracle::shouldSerialize(Variable const *var) {
  return serializeVar_O(const_cast<Variable*>(var));
}
