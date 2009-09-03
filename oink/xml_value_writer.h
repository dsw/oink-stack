// see License.txt for copyright and terms of use

// Serialization of [Abstract] Values to Xml.

#ifndef XML_VALUE_WRITER_H
#define XML_VALUE_WRITER_H

#include "xml_type_id.h"        // IdentityManager
#include "xml_writer.h"         // XmlWriter
#include "xml_type_writer.h"    // XmlTypeWriter
#include "oink_var.h"           // Variable_O
#include "value.h"              // Value
// #include "Lib/union_find_map.h" // UnionFindMap

// a type for predicates for filtering Varibles
typedef bool VarPredicate(Variable *var);

// This class provides generic traversal functionality as well has
// having build-in serialization functionality.  FIX: It would be
// convenient if I could turn off the printing, but that is a little
// hard to do at the moment.
class XmlValueWriter : public XmlTypeWriter {
public:

  class XVW_SerializeOracle : public XTW_SerializeOracle {
  public:
    virtual bool shouldSerialize(Value const *x) {return true;}
    virtual bool shouldSerialize(FVEllipsisHolder const *x) {return true;}
    virtual bool shouldSerialize(Variable const *x);

#define Delegate2super_shouldSerialize(TYPE) \
virtual bool shouldSerialize(TYPE const *x) {return XTW_SerializeOracle::shouldSerialize(x);}

    Delegate2super_shouldSerialize(Type)
    Delegate2super_shouldSerialize(CompoundType)
    Delegate2super_shouldSerialize(FunctionType::ExnSpec)
    Delegate2super_shouldSerialize(EnumType::Value)
    Delegate2super_shouldSerialize(BaseClass)
      // above
//     Delegate2super_shouldSerialize(Variable)
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
    template<class T> bool shouldSerialize(StringSObjDict<T> const *) {return true;}
#undef Delegate2super_shouldSerialize

  };

  // // predicate for filtering for vars we like
  // VarPredicate *varPred;
  // // predicate for filtering for values we like
  // ValuePredicate *valuePred;

  // for providing generic traversal functionality
  ValueVisitor *valueVisitor;

  public:
  XmlValueWriter(IdentityManager &idmgr0,
                 // VarPredicate *varPred0, ValuePredicate *valuePred0,
                 ASTVisitor *astVisitor0, ValueVisitor *valueVisitor,
                 std::ostream *out0, int &depth0, bool indent0,
                 XVW_SerializeOracle *serializeOracle0);
  virtual ~XmlValueWriter() {}

  public:

  // this is is now implicit in the fact that we inherit from
  // XmlTypeWriter
//   virtual void toXml(Type *t);

  // NOTE: this tagName_Value(), tagName_Variable() idiom works only
  // because the value classes do not actually have any more data than
  // their oink superclasses.

  // serialize values
  virtual char const * const tagName_Value(Value *var0);
  virtual void toXml(Value *v);
  virtual void toXml(FVEllipsisHolder *fveh);

  virtual char const * const tagName_Variable();
  // NOTE: this one overrides the same method in the superclass
  // XmlTypeWriter
  virtual void toXml(Variable *var);

  // the whole list of extern variables is held in one of these
  virtual void toXml_externVars(TailList<Variable_O> *list);

  virtual void toXml(BucketOValues *bv);

  // NOTE: these must also be overridden since overriding the one
  // above for Variable hides the whole overload set
#define Delegate2super_toXml(TYPE) virtual void toXml(TYPE obj) { XmlTypeWriter::toXml(obj); }
  Delegate2super_toXml(ObjList<STemplateArgument> *)
  Delegate2super_toXml(Type *)
  Delegate2super_toXml(AtomicType *)
  Delegate2super_toXml(CompoundType *) // disambiguates the overloading
  // this one we define above:
//   Delegate2super_toXml(Variable *)
  Delegate2super_toXml(OverloadSet *)
  Delegate2super_toXml(BaseClass *)
  Delegate2super_toXml(BaseClassSubobj *)
  Delegate2super_toXml(Scope *)
  Delegate2super_toXml(STemplateArgument *)
  Delegate2super_toXml(TemplateInfo *)
  Delegate2super_toXml(InheritedTemplateParams *)
#undef Delegate2super_toXml

  private:
  void toXml_Value_properties(Value *value);
  void toXml_Value_subtags(Value *value);
};

#endif // XML_VALUE_WRITER_H
