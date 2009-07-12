// xml_type_writer.h            see license.txt for copyright and terms of use

// Serialization for the type system, template system, and variables.

#ifndef XML_TYPE_WRITER_H
#define XML_TYPE_WRITER_H

#include "cc_type.h"            // Type
#include "template.h"           // Template stuff is only forward-declared in cc_type.h
#include "xml_writer.h"         // XmlWriter
#include "xml_type_id.h"        // identity_decl(Type) etc.
#include "cc_ast.h"             // XmlAstWriter_AstVisitor

class OverloadSet;
class ASTVisitor;

//  char const *toXml(CompoundType::Keyword id);
//  string toXml(FunctionFlags id);
//  char const *toXml(ScopeKind id);
//  char const *toXml(STemplateArgument::Kind id);


class XmlTypeWriter : public XmlWriter {
  public:
  ASTVisitor *astVisitor;       // for launching sub-traversals of AST we encounter in the Types

  class XTW_SerializeOracle {
  public:
    virtual ~XTW_SerializeOracle() {}
    virtual bool shouldSerialize(Type const *) {return true;}
    virtual bool shouldSerialize(CompoundType const *) {return true;}
    virtual bool shouldSerialize(FunctionType::ExnSpec const *) {return true;}
    virtual bool shouldSerialize(EnumType::Value const *) {return true;}
    virtual bool shouldSerialize(BaseClass const *) {return true;}
    virtual bool shouldSerialize(Variable const *) {return true;}
    virtual bool shouldSerialize(OverloadSet const *) {return true;}
    virtual bool shouldSerialize(STemplateArgument const *) {return true;}
    virtual bool shouldSerialize(TemplateInfo const *) {return true;}
    virtual bool shouldSerialize(InheritedTemplateParams const *) {return true;}
    // deal with Scott's multiple-inheritance funkyness
    virtual bool shouldSerialize(AtomicType const *obj);
    virtual bool shouldSerialize(Scope const *obj);
    // FIX: dsw: how do I make these virtual??  I don't have to override
    // them right now, but they should be virtual; perhaps that is not
    // possible because it would be too hard for the compiler to compute
    // the vtable
    template<class T> bool shouldSerialize(ObjList<T> const *) {return true;}
    template<class T> bool shouldSerialize(SObjList<T> const *) {return true;}
    template<class T> bool shouldSerialize(StringRefMap<T> const *) {return true;}
    template<class T> bool shouldSerialize(StringObjDict<T> const *) {return true;}
  };

  XTW_SerializeOracle *serializeOracle_m;

  public:
  XmlTypeWriter(IdentityManager &idmgr0, ASTVisitor *astVisitor0,
                ostream *out0, int &depth0, bool indent0,
                XTW_SerializeOracle *serializeOracle0);
  virtual ~XmlTypeWriter() {}

  public:
  // in the AST
  virtual void toXml(ObjList<STemplateArgument> *list);

  virtual void toXml(Type *t);
  virtual void toXml(AtomicType *atom);
  virtual void toXml(CompoundType *ct); // disambiguates the overloading
  void toXml_Variable_properties(Variable *var);
  void toXml_Variable_subtags(Variable *var);
  // dsw: For Oink it matters that this one is virtual; the rest are
  // just for consistency as I have to override all of the other
  // methods named toXml() at the same time as overriding one hides
  // the whole overload set.
  virtual void toXml(Variable *var);

  void toXml_FunctionType_ExnSpec(void /*FunctionType::ExnSpec*/ *exnSpec);

  void toXml_EnumType_Value(void /*EnumType::Value*/ *eValue0);
  void toXml_NamedAtomicType_properties(NamedAtomicType *nat);
  void toXml_NamedAtomicType_subtags(NamedAtomicType *nat);

  virtual void toXml(OverloadSet *oload);

  virtual void toXml(BaseClass *bc);
  void toXml_BaseClass_properties(BaseClass *bc);
  void toXml_BaseClass_subtags(BaseClass *bc);
  virtual void toXml(BaseClassSubobj *bc);

  virtual void toXml(Scope *scope);
  void toXml_Scope_properties(Scope *scope);
  void toXml_Scope_subtags(Scope *scope);

  virtual void toXml(STemplateArgument *sta);
  virtual void toXml(TemplateInfo *ti);
  virtual void toXml(InheritedTemplateParams *itp);
  void toXml_TemplateParams_properties(TemplateParams *tp);
  void toXml_TemplateParams_subtags(TemplateParams *tp);
};

// print out type annotations for every ast node that has a type
class XmlTypeWriter_AstVisitor : public XmlAstWriter_AstVisitor {
//    ostream &out;                 // for the <Link/> tags
  XmlTypeWriter &ttx;

  public:
  XmlTypeWriter_AstVisitor
    (XmlTypeWriter &ttx0,
     ostream &out0,
     int &depth0,
     bool indent0 = false);

  virtual bool shouldSerialize(Variable const *var) {
    return !ttx.serializeOracle_m || ttx.serializeOracle_m->shouldSerialize(var);
  }
  IdentityManager &getIdMgr() { return ttx.idmgr; }

  // **** visit methods
  virtual bool visitTranslationUnit(TranslationUnit *unit);
  virtual bool visitTypeSpecifier(TypeSpecifier *ts);
  virtual bool visitFunction(Function *f);
  virtual bool visitMemberInit(MemberInit *memberInit);
  virtual bool visitBaseClassSpec(BaseClassSpec *bcs);
  virtual bool visitDeclarator(Declarator *d);
  virtual bool visitExpression(Expression *e);
#ifdef GNU_EXTENSION
  virtual bool visitASTTypeof(ASTTypeof *a);
#endif // GNU_EXTENSION
  virtual bool visitPQName(PQName *pqn);
  virtual bool visitEnumerator(Enumerator *e);
  virtual bool visitInitializer(Initializer *e);
  virtual bool visitTemplateParameter(TemplateParameter *tparam);
  virtual bool visitHandler(Handler *h);
};

#endif // XML_TYPE_WRITER_H
