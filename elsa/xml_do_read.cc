// xml_do_read.cc          see license.txt for copyright and terms of use

#include "xml_do_read.h"        // this module
#include "fstream.h"            // ifstream
#include "xml_lexer.h"          // XmlLexer
#include "xml_file_reader.h"    // XmlFileReader
#include "xml_type_reader.h"    // XmlTypeReader
#include "xml_ast_reader.h"     // XmlAstReader

class TranslationUnit;


TranslationUnit *xmlDoRead(StringTable &strTable, char const *inputFname) {
  // make reader manager
  ifstream in(inputFname);
  XmlLexer lexer;
  lexer.inputFname = inputFname;
  lexer.restart(&in);
  XmlReaderManager manager(lexer, strTable);
  manager.inputFname = inputFname;

  // prevent the SourceLocManager from looking at files in the file
  // system
  sourceLocManager->mayOpenFiles = false;

  // make and register file reader
  manager.registerReader(new XmlFileReader);

  // make and register ast reader
  manager.registerReader(new XmlAstReader);

  // make and register type reader
  manager.registerReader(new XmlTypeReader);

  // read until we get a translation unit tag; FIX: not sure what
  // happens if the last tag is not a TranslationUnit
  while(true) {
    manager.parseOneTopLevelTag();
    if (lexer.haveSeenEof()) {
      manager.xmlUserFatalError("unexpected EOF");
    }
    int lastKind = manager.getLastKind();
    if (lastKind == XTOK_List_files) {
      // complete the link graph so that the FileData object is
      // complete
      manager.satisfyLinks();
      ObjList<SourceLocManager::FileData> *files =
        (ObjList<SourceLocManager::FileData>*) manager.getLastNode();
      FOREACH_OBJLIST_NC(SourceLocManager::FileData, *files, iter) {
        SourceLocManager::FileData *fileData = iter.data();
        if (!fileData->complete()) {
          manager.xmlUserFatalError("missing attributes to File tag");
        }
        sourceLocManager->loadFile(fileData);
      }
      // Note: 'files' owns the FileDatas so it will delete them for us.
      delete files;
    } else if (lastKind == XTOK_TranslationUnit) {
      break;                    // we are done
    } else {
      manager.xmlUserFatalError("illegal top-level tag");
    }
  }

  // complete the link graph
  manager.satisfyLinks();

//    if (manager.getLastKind() != XTOK_TranslationUnit) {
//      manager.xmlUserFatalError("top tag is not a TranslationUnit");
//    }
  TranslationUnit *tunit = (TranslationUnit*) manager.getLastNode();

  return tunit;
}
