// xml_file_writer.h            see license.txt for copyright and terms of use

// Serialization of file information for purposes of capturing the
// state of the SourceLocManager.

#ifndef XML_FILE_WRITER_H
#define XML_FILE_WRITER_H

#include "sobjset.h"            // SObjSet
#include "objlist.h"            // ObjList
#include "srcloc.h"             // SourceLocManager
#include "xml_writer.h"         // XmlWriter
#include "hashline.h"           // HashLineMap

class XmlFileWriter : public XmlWriter {
public:

  class XFW_SerializeOracle {
  public:
    virtual ~XFW_SerializeOracle() {}
    virtual bool shouldSerialize(SourceLocManager::File const *) {return true;}
    virtual bool shouldSerialize(HashLineMap const *) {return true;}
    virtual bool shouldSerialize(HashLineMap::HashLine const *) {return true;}
  };

  XFW_SerializeOracle *serializeOracle_m;

  XmlFileWriter(IdentityManager &idmgr0, ostream *out0, int &depth0, bool indent0,
                XFW_SerializeOracle *serializeOracle0);
  virtual ~XmlFileWriter() {}

  void toXml(SourceLocManager::FileList &files);
  void toXml(SourceLocManager::File *file);
  // this is an exception to the generic toXml() mechanism since
  // lineLengths are not self-contained
  void toXml_lineLengths(SourceLocManager::File *file);
  void toXml(HashLineMap *hashLines);
  void toXml(HashLineMap::HashLine *hashLine);
};

#endif // XML_FILE_WRITER_H
