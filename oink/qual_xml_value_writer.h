// see License.txt for copyright and terms of use

// Serialization of [Abstract] Values to Xml taking into account qual
// considerations.

#ifndef QUAL_XML_VALUE_WRITER_H
#define QUAL_XML_VALUE_WRITER_H

#include "xml_value_writer.h"   // XmlValueWriter
#include "qual_var.h"           // Variable_Q

class XmlValueWriter_Q : public XmlValueWriter {
public:

  class XVWQ_SerializeOracle : public XVW_SerializeOracle {
  public:
    virtual bool shouldSerialize(Value const *value);
    virtual bool shouldSerialize(FVEllipsisHolder const *x) {return true;}
    virtual bool shouldSerialize(Variable const *var);

#define Delegate2super_shouldSerialize(TYPE) \
virtual bool shouldSerialize(TYPE const *x) {return XVW_SerializeOracle::shouldSerialize(x);}

    Delegate2super_shouldSerialize(Type)
    Delegate2super_shouldSerialize(CompoundType)
    Delegate2super_shouldSerialize(FunctionType::ExnSpec)
    Delegate2super_shouldSerialize(EnumType::Value)
    Delegate2super_shouldSerialize(BaseClass)
    // above:
//   Delegate2super_shouldSerialize(Variable)
    Delegate2super_shouldSerialize(OverloadSet)
    Delegate2super_shouldSerialize(STemplateArgument)
    Delegate2super_shouldSerialize(TemplateInfo)
    Delegate2super_shouldSerialize(InheritedTemplateParams)
    // deal with Scott's multiple-inheritance funkyness
    Delegate2super_shouldSerialize(AtomicType)
    Delegate2super_shouldSerialize(Scope)

    // virtual bool shouldSerialize(UFM_CVA_BOV const *x) {return true;}
    // virtual bool shouldSerialize(PM_CVA_BOV const *x) {return true;}
    // virtual bool shouldSerialize(UF_CVA const *x) {return true;}
    // virtual bool shouldSerialize(PM_CVA_CVA const *x) {return true;}
    virtual bool shouldSerialize(BucketOValues const *x) {return true;}

    // FIX: dsw: how do I make these virtual??  I don't have to override
    // them right now, but they should be virtual; perhaps that is not
    // possible because it would be too hard for the compiler to compute
    // the vtable
    template<class T> bool shouldSerialize(ObjList<T> const *) {return true;}
    template<class T> bool shouldSerialize(SObjList<T> const *) {return true;}
    template<class T> bool shouldSerialize(StringRefMap<T> const *) {return true;}
    template<class T> bool shouldSerialize(StringObjDict<T> const *) {return true;}
#undef Delegate2super_shouldSerialize
  };


  XmlValueWriter_Q(IdentityManager &idmgr,
                   // VarPredicate *varPred0, ValuePredicate *valuePred0,
                   ASTVisitor *astVisitor0, ValueVisitor *valueVisitor,
                   ostream *out0, int &depth0, bool indent0,
                   XVW_SerializeOracle *srzOracle0);
  virtual ~XmlValueWriter_Q() {}

  virtual char const * const tagName_Value(Value *var0);
  virtual char const * const tagName_Variable();
  virtual void toXml_externVars(TailList<Variable_O> *list);
};

#endif // QUAL_XML_VALUE_WRITER_H
