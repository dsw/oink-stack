// xml_do_read.h         see license.txt for copyright and terms of use

#ifndef XML_DO_READ_H
#define XML_DO_READ_H

class TranslationUnit;
class StringTable;

TranslationUnit *xmlDoRead(StringTable &strTable, char const *inputFname);

#endif // XML_DO_READ_H
