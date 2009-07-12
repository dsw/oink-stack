// xml_type_reader.h            see license.txt for copyright and terms of use

// De-serialization for the type system, template system, and
// variables.

#ifndef XML_TYPE_READER_H
#define XML_TYPE_READER_H

#include "cc_type.h"            // Type
#include "variable.h"
#include "template.h"           // Template stuff is only forward-declared in cc_type.h
#include "xml_reader.h"         // XmlReader

//  void fromXml(CompoundType::Keyword &out, rostring str);
//  void fromXml(FunctionFlags &out, rostring str);
//  void fromXml(ScopeKind &out, rostring str);
//  void fromXml(STemplateArgument::Kind &out, rostring str);


class XmlTypeReader : public XmlReader {
//    BasicTypeFactory &tFac;

  public:
  // Parse a tag: construct a node for a tag
  virtual void *ctorNodeFromTag(int tag);

  // Parse an attribute: register an attribute into the current node
  virtual bool registerAttribute(void *target, int kind, int attr, char const *yytext0);

  virtual bool registerStringToken(void *target, int kind, char const *yytext0);

  // implement an eq-relation on tag kinds by mapping a tag kind to a
  // category
  virtual bool kind2kindCat(int kind, KindCategory *kindCat);

  // **** Generic Convert
  virtual bool recordKind(int kind, bool& answer);

  // cast a pointer to the pointer type we need it to be
  virtual bool callOpAssignToEmbeddedObj(void *obj, int kind, void *target);
  virtual bool upcastToWantedType(void *obj, int kind, void **target, int targetKind);

  private:
  // Types
  void registerAttr_CVAtomicType       (CVAtomicType *,        int attr, char const *strValue);
  void registerAttr_PointerType        (PointerType *,         int attr, char const *strValue);
  void registerAttr_ReferenceType      (ReferenceType *,       int attr, char const *strValue);
  void registerAttr_FunctionType       (FunctionType *,        int attr, char const *strValue);
  void registerAttr_FunctionType_ExnSpec
    (FunctionType::ExnSpec *, int attr, char const *strValue);
  void registerAttr_ArrayType          (ArrayType *,           int attr, char const *strValue);
  void registerAttr_PointerToMemberType(PointerToMemberType *, int attr, char const *strValue);
  void registerAttr_DependentSizedArrayType(DependentSizedArrayType *, int attr, char const *strValue);

  // AtomicTypes
  bool registerAttr_NamedAtomicType_super(NamedAtomicType *,   int attr, char const *strValue);
  void registerAttr_SimpleType         (SimpleType *,          int attr, char const *strValue);
  void registerAttr_CompoundType       (CompoundType *,        int attr, char const *strValue);
  void registerAttr_EnumType           (EnumType *,            int attr, char const *strValue);
  void registerAttr_EnumType_Value     (EnumType::Value *,     int attr, char const *strValue);
  void registerAttr_TypeVariable       (TypeVariable *,        int attr, char const *strValue);
  void registerAttr_PseudoInstantiation(PseudoInstantiation *, int attr, char const *strValue);
  void registerAttr_DependentQType     (DependentQType *,      int attr, char const *strValue);

  // other
  public:
  bool registerAttr_Variable_super     (Variable *,            int attr, char const *strValue);

  private:
  void registerAttr_Variable           (Variable *,            int attr, char const *strValue);
  bool registerAttr_Scope_super        (Scope *,               int attr, char const *strValue);
  void registerAttr_Scope              (Scope *,               int attr, char const *strValue);
  bool registerAttr_BaseClass_super    (BaseClass *,           int attr, char const *strValue);
  void registerAttr_BaseClass          (BaseClass *,           int attr, char const *strValue);
  void registerAttr_BaseClassSubobj    (BaseClassSubobj *,     int attr, char const *strValue);
  void registerAttr_OverloadSet        (OverloadSet *,         int attr, char const *strValue);
  void registerAttr_STemplateArgument  (STemplateArgument *,   int attr, char const *strValue);
  bool registerAttr_TemplateParams_super(TemplateParams *obj,  int attr, char const *strValue);
  void registerAttr_TemplateInfo       (TemplateInfo *,        int attr, char const *strValue);
  void registerAttr_InheritedTemplateParams(InheritedTemplateParams*,
                                                               int attr, char const *strValue);
};

#endif // XML_TYPE_READER_H
