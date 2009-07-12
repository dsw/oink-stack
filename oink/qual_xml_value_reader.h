// see License.txt for copyright and terms of use

// Specialization of the ReadXml framework that reads in XML for
// serialized qual Values.

#ifndef QUAL_XML_VALUE_READER_H
#define QUAL_XML_VALUE_READER_H

#include "xml_value_reader.h"   // XmlValueReader
#include "qual_var.h"           // Variable_Q
#include "value.h"              // Value

class XmlValueReader_Q : public XmlValueReader {
  public:
  XmlValueReader_Q(XmlTypeReader &typeXmlReader0)
    : XmlValueReader(typeXmlReader0)
  {}
  virtual ~XmlValueReader_Q() {}

  virtual void *ctorNodeFromTag(int tag);
  virtual bool kind2kindCat(int kind, KindCategory *kindCat);
  virtual bool recordKind(int kind, bool& answer);
  virtual bool registerAttribute(void *target, int kind, int attr, char const *yytext0);
};

#endif // QUAL_XML_VALUE_READER_H
