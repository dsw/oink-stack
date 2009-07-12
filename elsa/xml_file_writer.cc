// xml_file_writer.cc            see license.txt for copyright and terms of use

#include "xml_file_writer.h"    // this module
#include "xml_writer.h"         // serialization support
#include "asthelp.h"            // xmlAttrQuote()
#include "sobjset.h"            // SObjSet

#define serializeOracle serializeOracle_m

XmlFileWriter::XmlFileWriter(IdentityManager &idmgr0,
                             ostream *out0, int &depth0, bool indent0,
                             XmlFileWriter::XFW_SerializeOracle *serializeOracle0)
  : XmlWriter(idmgr0, out0, depth0, indent0),
    serializeOracle(serializeOracle0)
{}

void XmlFileWriter::toXml(SourceLocManager::FileList &files)
{
  travObjList0(files, files, SourceLocManager::File, FOREACH_OBJARRAYSTACK_NC, ObjArrayStack);

  // FOREACH_OBJLIST_NC(SourceLocManager::File, files, iter) {
  //   SourceLocManager::File *file = iter.data();
  //   toXml(file);
  // }
}

void XmlFileWriter::toXml(SourceLocManager::File *file)
{
  // idempotency
  if (idmgr.printed(file)) return;

  unsigned char *lineLengths = file->serializationOnly_get_lineLengths();

  XmlTagPrinter tagPrinter(*this);
  if (writingP()) {
    tagPrinter.printOpenTag("File", file);

    // **** attributes
    printStrRef(name, file->name.c_str());
    printXml_int(numChars, file->numChars);
    printXml_int(numLines, file->numLines);

  // this doesn't work because lineLengths is private
//    printPtr(file, lineLengths);

    // FIX: this special situation just breaks all the macros so we do
    // it manually
    newline();
    *out << "lineLengths=";
    outputXmlPointerQuoted(*out, "FI", idmgr.uniqueId(lineLengths));

    printPtr(file, hashLines);
    tagPrinter.tagEnd();
  }

  // **** subtags
  // NOTE: we do not use the trav() macro as we call a non-standard
  // method name; see note at the method declaration
  if (lineLengths) {
    // NOTE: we pass the file instead of the lineLengths
    toXml_lineLengths(file);
  }
  trav(file->hashLines);
}

void XmlFileWriter::toXml_lineLengths(SourceLocManager::File *file)
{
  // NOTE: no idempotency check is needed as the line lengths are
  // one-to-one with the Files.
  XmlTagPrinter tagPrinter(*this);
  if (writingP()) {
    unsigned char *lineLengths = file->serializationOnly_get_lineLengths();
    int lineLengthsSize = file->serializationOnly_get_lineLengthsSize();
    // NOTE: can't do this since we would have to implement dispatch on
    // a pointer to unsigned chars, which is too general; we need
    // LineLengths to be their own class.
//    openTagWhole(LineLengths, lineLengths);
    tagPrinter.readyTag("LineLengths");
    *out << "<LineLengths _id=";
    outputXmlPointerQuoted(*out, "FI", idmgr.uniqueId(lineLengths));
    *out << " size='" << lineLengthsSize;
    *out << "'>";

    // **** sub-data
    // Note: This simple whitespace-separated list is the suggested
    // output format for lists of numbers in XML:
    // http://www.w3.org/TR/xmlschema-0/primer.html#ListDt
    //
    // Note also that I do not bother to indent blocks of data between
    // tags, just the tags themselves.
    for (int i=0; i<lineLengthsSize; ++i) {
      if (i%20 == 0) *out << '\n';
      else *out << ' ';
      *out << static_cast<int>(lineLengths[i]);
    }
  }
}

void XmlFileWriter::toXml(HashLineMap *hashLines)
{
  // idempotency
  if (idmgr.printed(hashLines)) return;

  ArrayStack<HashLineMap::HashLine> &directives = hashLines->serializationOnly_get_directives();

  XmlTagPrinter tagPrinter(*this);
  if (writingP()) {
    tagPrinter.printOpenTag("HashLineMap", hashLines);

    // **** attributes
    string &ppFname = hashLines->serializationOnly_get_ppFname();
    printStrRef(ppFname, ppFname.c_str());

    // NOTE: can't do this because it is private; FIX: I have inlined
    // "FI" here.
//    printEmbed(hashLines, directives);
    newline();
    *out << "directives=";
    outputXmlPointerQuoted(*out, "FI", idmgr.uniqueId(&directives));
    tagPrinter.tagEnd();
  }

  // **** subtags
  // FIX: again, it is private so I inline the macro
//    travArrayStack(hashLines, HashLineMap, directives, HashLine);
  if (!idmgr.printed(&directives)) {
    XmlTagPrinter tagPrinter2(*this);
    if (writingP()) {
      tagPrinter2.printOpenTag("List_HashLineMap_directives", &directives);
      tagPrinter2.tagEnd();
    }
    FOREACH_ARRAYSTACK_NC(HashLineMap::HashLine, directives, iter) {
      travListItem(iter.data());
    }
  }
}

void XmlFileWriter::toXml(HashLineMap::HashLine *hashLine)
{
  // idempotency
  if (idmgr.printed(hashLine)) return;

  XmlTagPrinter tagPrinter(*this);
  if (writingP()) {
    tagPrinter.printOpenTag("HashLine", hashLine);

    // **** attributes
    printXml_int(ppLine, hashLine->ppLine);
    printXml_int(origLine, hashLine->origLine);
    printStrRef(origFname, hashLine->origFname);
    tagPrinter.tagEnd();
  }
}
