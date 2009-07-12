// see License.txt for copyright and terms of use

// Specialization of the ReadXml framework that reads in XML for
// serialized Values.

#ifndef XML_VALUE_READER_H
#define XML_VALUE_READER_H

#include "xml_reader.h"         // XmlReader
#include "xml_type_reader.h"    // XmlTypeReader
#include "oink_var.h"           // Variable_O
#include "value.h"              // Value
// #include "Lib/union_find_map.h" // UnionFindMap, UnionFind

// this is duplicated in xml_value_writer.h
// typedef UnionFindMap<CVAtomicValue, BucketOValues> UFM_CVA_BOV;
// typedef PtrMap<CVAtomicValue, BucketOValues> PM_CVA_BOV;
// typedef UnionFind<CVAtomicValue> UF_CVA;
// typedef PtrMap<CVAtomicValue, CVAtomicValue> PM_CVA_CVA;

// parse Types and Variables serialized as XML
class XmlValueReader : public XmlReader {
  XmlTypeReader &typeXmlReader;

  public:
  XmlValueReader(XmlTypeReader &typeXmlReader0)
    : XmlReader()
    , typeXmlReader(typeXmlReader0)
  {}

  // Parse a tag: construct a node for a tag
  virtual void *ctorNodeFromTag(int tag);
  /*not virtual*/ void *ctorNodeFromTag_reusable(int tag);

  // Parse an attribute: register an attribute into the current node
  virtual bool registerAttribute(void *target, int kind, int attr, char const *yytext0);
  /*not virtual*/ bool registerAttribute_reusable
    (void *target, int kind, int attr, char const *yytext0);

  virtual bool registerStringToken(void *target, int kind, char const *yytext0);

  // implement an eq-relation on tag kinds by mapping a tag kind to a
  // category
  virtual bool kind2kindCat(int kind, KindCategory *kindCat);

  // **** Generic Convert
  virtual bool recordKind(int kind, bool& answer);

  // cast a pointer to the pointer type we need it to be
  virtual bool callOpAssignToEmbeddedObj(void *obj, int kind, void *target);
  virtual bool upcastToWantedType(void *obj, int kind, void **target, int targetKind);

  protected:
  // Values
  bool registerAttr_Value_super         (Value *obj,             int attr, char const *strValue);
  void registerAttr_CVAtomicValue       (CVAtomicValue *,        int attr, char const *strValue);
  void registerAttr_PointerValue        (PointerValue *,         int attr, char const *strValue);
  void registerAttr_ReferenceValue      (ReferenceValue *,       int attr, char const *strValue);
  void registerAttr_FunctionValue       (FunctionValue *,        int attr, char const *strValue);
  void registerAttr_ArrayValue          (ArrayValue *,           int attr, char const *strValue);
  void registerAttr_PointerToMemberValue(PointerToMemberValue *, int attr, char const *strValue);
  void registerAttr_FVEllipsisHolder    (FVEllipsisHolder *,     int attr, char const *strValue);

  // other
  void registerAttr_Variable_O          (Variable_O *,           int attr, char const *strValue);

  void registerAttr_BucketOValues(BucketOValues *obj, int attr, char const *strValue);
};

#endif // XML_VALUE_READER_H
