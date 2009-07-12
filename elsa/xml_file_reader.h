// xml_file_reader.h            see license.txt for copyright and terms of use

// De-serialization of file information for purposes of capturing the
// state of the SourceLocManager.

#ifndef XML_FILE_READER_H
#define XML_FILE_READER_H

#include "xml_reader.h"
#include "astlist.h"


class XmlFileReader : public XmlReader {
  public:
  XmlFileReader() {}
  virtual ~XmlFileReader() {}

  virtual void *ctorNodeFromTag(int tag);
  virtual bool registerAttribute(void *target, int kind, int attr, char const *yytext0);
  virtual bool registerStringToken(void *target, int kind, char const *yytext0);
  virtual bool kind2kindCat(int kind, KindCategory *kindCat);
  // **** Generic Convert
  virtual bool recordKind(int kind, bool& answer);
  virtual bool callOpAssignToEmbeddedObj(void *obj, int kind, void *target);
  virtual bool upcastToWantedType(void *obj, int kind, void **target, int targetKind);
  virtual bool appendToArrayStack(void *arrayStack, void *obj, int listKind);
};

#endif // XML_FILE_READER_H
