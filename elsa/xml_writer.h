
// xml_writer.h            see license.txt for copyright and terms of use

// Support for XML serialization

// FIX: this module should eventually go into the ast repository.

// FIX: many of these macros could be changed into function templates.

#ifndef XML_WRITER_H
#define XML_WRITER_H

#include "strtable.h"           // StringRef
#include "xmlhelp.h"            // toXml_int etc.
#include "strmap.h"             // StringRefMap
#include "sobjset.h"            // SObjSet
#include "xml_type_id.h"        // IdentityManager

// FIX: this is a hack; I want some place to set a flag to tell me if
// I want the xml serialization of name-maps to be canonical (names in
// sorted order)
extern bool sortNameMapDomainWhenSerializing;


// to Xml for enums
#define PRINTENUM(X) case X: return #X
#define PRINTFLAG(X) if (id & (X)) b << #X

// Manage the output stream.
//
// It takes a SerializeOracle which tells whether to serialize; if it is NULL
// we serialize all.
//
// The IdentityManager encapsulates the functionality to generate unique IDs
// and also, importantly, to check which nodes have been visited.  A new
// IdentityManager must be passed in for every new traversal.
//
// If OUT is NULL then we don't actually write anything; we just traverse.
// (This used to be only used for serialization, but now we also reuse it to
// traverse the AST in the same order.  TODO: rename XmlWriter; even better
// refactor xml writing in visitors.)

class XmlWriter {
public:
  IdentityManager &idmgr;       // unique id manager

protected:
  std::ostream *out;            // output stream to which to print

  int &depth;                   // ref so we can share our indentation depth with other printers
  bool indent;                  // should we print indentation?

public:
  XmlWriter(IdentityManager &idmgr0, std::ostream *out0, int &depth0, bool indent0);

  // whether we are writing or not.
  bool writingP() const { return out != NULL; }

protected:
  // print a newline and indent if the user wants indentation; NOTE:
  // the convention is that you don't print a newline until you are
  // *sure* you have something to print that goes onto the next line;
  // that is, most lines do *not* end in a newline
  void newline();
  friend class XmlTagPrinter;
};

// Manage XML tags:
//   Either print both the open and close tags, or neither.  The close tag is
//   automatically printed on destruction iff the open tag was printed.
class XmlTagPrinter {
  // NOTE: you must pass in a constant string, not e.g. a str.c_str(), because
  // we need it until the end of the scope, and we do not copy the string.
  const char *tagname;
  XmlWriter &ttx;

public:
  XmlTagPrinter(XmlWriter &ttx0) : tagname(NULL), ttx(ttx0)
  {
  }

  void readyTag(const char *tagname0) {
    xassert(ttx.writingP());
    xassert(tagname == NULL); // only call readyTag once since only one closeTag
    tagname = tagname0;
    ttx.depth++;
    newline();
  }

  template <class T>
  void printOpenTag(const char *tagname0, T const &obj) {
    readyTag(tagname0);

    // idPrefix and uniqueId are overloaded on T.
    out() << '<' << tagname << " _id=";
    outputXmlPointerQuoted(out(),
                           ttx.idmgr.idPrefix(obj),
                           ttx.idmgr.uniqueId(obj));
  }

  template <class T>
  void printNameMapItemOpenTag(const char *objname, T const &target) {
    readyTag("_NameMap_Item");
    out() << "<_NameMap_Item"
          << " name=";
    outputXmlAttrQuoted(out(), objname);
    out() << " item=";
    outputXmlPointerQuoted(out(),
                           ttx.idmgr.idPrefix(target),
                           ttx.idmgr.uniqueId(target));
    out() << '>';
  }

  template <class T>
  void printListItemOpenTag(T const &target) {
    readyTag("_List_Item");
    out() << "<_List_Item item=";
    outputXmlPointerQuoted(out(),
                           ttx.idmgr.idPrefix(target),
                           ttx.idmgr.uniqueId(target));
    out() << '>';
  }

  template <class T>
  void printMapItemOpenTag(const char *key, T const &value) {
    readyTag("_Map_Item");
    out() << "<_Map_Item"
          << " key=";
    outputXmlAttrQuoted(out(), key);
    out() << " item=";
    outputXmlPointerQuoted(out(), ttx.idmgr.idPrefix(value), ttx.idmgr.uniqueId(value));
    out() << '>';
  }

  template <class T>
  void printMapItemOpenTag(T const &key, T const &value) {
    readyTag("_Map_Item");
    out() << "<_Map_Item"
          << " key=";
    outputXmlPointerQuoted(out(), ttx.idmgr.idPrefix(key), ttx.idmgr.uniqueId(key));
    out() << " item=";
    outputXmlPointerQuoted(out(), ttx.idmgr.idPrefix(value), ttx.idmgr.uniqueId(value));
    out() << '>';
  }

  // template <class T>
  // void printStrRef0(const char *field, T const &target) {
  //   if (target) {
  //     newline();
  //     out() << field << '=' << xmlAttrQuote(target);
  //   }
  // }

  // for ending the open tag
  void tagEnd() {
    xassert(ttx.writingP());
    out() << '>';
  }

  ~XmlTagPrinter() {
    if (ttx.writingP())
      closeTag();
  }

  void closeTag() {
    xassert(tagname != NULL);
    newline();
    out() << '<' << '/' << tagname << '>';
    ttx.depth--;
  }

  std::ostream &out() { xassert(ttx.out); return *ttx.out; }

protected:
  void newline() { ttx.newline(); }

private:
  explicit XmlTagPrinter(XmlTagPrinter &); // prohibit
};

// manage indentation depth
class IncDec {
  int &x;
  public:
  explicit IncDec(int &x0) : x(x0) {++x;}
  private:
  explicit IncDec(const IncDec&); // prohibit
  public:
  ~IncDec() {--x;}
};

#define printThing0(NAME, VALUE)                                   \
  do {                                                             \
    *out << #NAME "=";                                             \
    outputXmlAttrQuoted(*out, VALUE);                              \
  } while(0)

#define printThing0_ne(NAME, VALUE)                               \
  do {                                                            \
    *out << #NAME "=";                                            \
    outputXmlAttrQuotedNoEscape(*out, VALUE);                     \
  } while(0)

#define printThing(NAME, RAW, VALUE)                                           \
  do {                                                                         \
    if ((RAW) && (!serializeOracle || serializeOracle->shouldSerialize(RAW))) { \
      newline();                                                               \
      printThing0(NAME, VALUE);                                                \
    }                                                                          \
  } while(0)

#define printThingAST(NAME, RAW, VALUE)                           \
  do {                                                            \
    if (astVisitor && RAW) {                                      \
      newline();                                                  \
      printThing0(NAME, VALUE);                                   \
    }                                                             \
  } while(0)

// TODO: refactor uniqueIdAST into IdentityManager
#define printPtr0(NAME, VALUE)                                    \
  do {                                                            \
    newline();                                                    \
    *out << #NAME "=";                                            \
    outputXmlPointerQuoted(*out, idmgr.idPrefix(VALUE),           \
                           idmgr.uniqueId(VALUE));                \
  } while(0)

#define printPtr1(NAME, VALUE)                                    \
  if (!serializeOracle ||                                         \
      serializeOracle->shouldSerialize(VALUE)) {                  \
    printPtr0(NAME, VALUE);                                       \
  }

// quarl 2006-05-28
//    Don't print attributes of pointers which point to NULL.
#define printPtr(BASE, MEM)                                       \
  do {                                                            \
    if ((BASE)->MEM) {                                            \
      printPtr1(MEM, (BASE)->MEM);                                \
    }                                                             \
  } while(0)

// NOTE: don't do this as there is no shouldSerialize infrastructre
// for AST nodes
//       if (!serializeOracle ||
//           serializeOracle->shouldSerialize((BASE)->MEM)) {
#define printPtrAST(BASE, MEM)                                    \
  do {                                                            \
    if (astVisitor && ((BASE)->MEM)) {                            \
      newline();                                                  \
      *out << #MEM "=";                                           \
      outputXmlPointerQuoted(*out, "AST",                         \
                             uniqueIdAST((BASE)->MEM));           \
    }                                                             \
  } while(0)

// print an embedded thing
#define printEmbed(BASE, MEM) printPtr1(MEM, (&((BASE)->MEM)))

// for unions where the member name does not match the xml name and we
// don't want the 'if'
#define printPtrUnion(BASE, MEM, NAME)                            \
  do {                                                            \
    if (!serializeOracle ||                                       \
        serializeOracle->shouldSerialize((BASE)->MEM)) {          \
      newline();                                                  \
      *out << #NAME "=";                                          \
      outputXmlPointerQuoted(*out, idmgr.idPrefix((BASE)->MEM),   \
                             idmgr.uniqueId((BASE)->MEM));        \
    }                                                             \
  } while(0)

// this is only used in one place
// NOTE: don't do this as there is no shouldSerialize infrastructre
// for AST nodes
//       if (!serializeOracle ||
//           serializeOracle->shouldSerialize((BASE)->MEM)) {
#define printPtrASTUnion(BASE, MEM, NAME)                         \
  do {                                                            \
    if (astVisitor) {                                             \
      newline();                                                  \
      *out << #NAME "=";                                          \
      outputXmlPointerQuoted(*out, "AST",                         \
                             uniqueIdAST((BASE)->MEM));           \
    }                                                             \
  } while(0)

#define printXml(NAME, VALUE)                                     \
  do {                                                            \
    newline();                                                    \
    printThing0(NAME, ::toXml(VALUE));                            \
  } while(0)

#define printXml_bool(NAME, VALUE)                                \
  do {                                                            \
    newline();                                                    \
    printThing0_ne(NAME, ::toXml_bool(VALUE));                    \
  } while(0)

#define printXml_int(NAME, VALUE)                                 \
  do {                                                            \
    newline();                                                    \
    printThing0_ne(NAME, ::toXml_int(VALUE));                     \
  } while(0)

#define printXml_SourceLoc(NAME, VALUE)                           \
  do {                                                            \
    newline();                                                    \
    printThing0(NAME, ::toXml_SourceLoc(VALUE));                  \
  } while(0)

// #define printStrRef(NAME, VALUE) tagPrinter.printStrRef0(#NAME, VALUE)

#define printStrRef(NAME, VALUE)                                  \
  do {                                                            \
    if (VALUE) {                                                  \
      newline();                                                  \
      *out << #NAME "=";                                          \
      outputXmlAttrQuoted(*out, VALUE);                           \
    }                                                             \
  } while(0)

// FIX: rename this; it also works for ArrayStacks
#define travObjList0(OBJ, TAGNAME, FIELDTYPE, ITER_MACRO, LISTKIND)            \
  do {                                                                         \
    if (!idmgr.printed(&OBJ)) {                                                \
      XmlTagPrinter tagPrinter(*this);                                         \
      if (writingP()) {                                                        \
        tagPrinter.printOpenTag("List_" #TAGNAME, &OBJ);                       \
        tagPrinter.tagEnd();                                                   \
      }                                                                        \
      ITER_MACRO(FIELDTYPE, const_cast<LISTKIND<FIELDTYPE>&>(OBJ), iter) {     \
        FIELDTYPE *obj0 = iter.data();                                         \
        if (!serializeOracle || serializeOracle->shouldSerialize(obj0)) {      \
          travListItem(obj0);                                                  \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  } while(0)

#define travObjList1(OBJ, BASETYPE, FIELD, FIELDTYPE, ITER_MACRO, LISTKIND) \
  travObjList0(OBJ, BASETYPE ##_ ##FIELD, FIELDTYPE, ITER_MACRO, LISTKIND)

#define travObjList_S(BASE, BASETYPE, FIELD, FIELDTYPE) \
  travObjList1(BASE->FIELD, BASETYPE, FIELD, FIELDTYPE, SFOREACH_OBJLIST_NC, SObjList)

#define travObjListPtr_S(BASE, BASETYPE, FIELD, FIELDTYPE) \
  travObjList1(*(BASE->FIELD), BASETYPE, FIELD, FIELDTYPE, SFOREACH_OBJLIST_NC, SObjList)

#define travObjList(BASE, BASETYPE, FIELD, FIELDTYPE) \
  travObjList1(BASE->FIELD, BASETYPE, FIELD, FIELDTYPE, FOREACH_OBJLIST_NC, ObjList)

// Not tested; put the backslash back after the first line
//  #define travArrayStack(BASE, BASETYPE, FIELD, FIELDTYPE)
//  travObjList1(BASE->FIELD, BASETYPE, FIELD, FIELDTYPE, FOREACH_ARRAYSTACK_NC, ArrayStack)

#define travObjList_standalone(OBJ, BASETYPE, FIELD, FIELDTYPE) \
  travObjList1(OBJ, BASETYPE, FIELD, FIELDTYPE, FOREACH_OBJLIST_NC, ObjList)

#define travStringRefMap0(OBJ, BASETYPE, FIELD, RANGETYPE)                     \
  do {                                                                         \
    if (!idmgr.printed(OBJ)) {                                                 \
      XmlTagPrinter tagPrinter(*this);                                         \
      if (writingP()) {                                                        \
        tagPrinter.printOpenTag("NameMap_" #BASETYPE "_" #FIELD, OBJ);         \
        tagPrinter.tagEnd();                                                   \
      }                                                                        \
      if (sortNameMapDomainWhenSerializing) {                                  \
        for(StringRefMap<RANGETYPE>::SortedKeyIter iter(*(OBJ));               \
            !iter.isDone(); iter.adv()) {                                      \
          RANGETYPE *obj = iter.value();                                       \
          if (!serializeOracle || serializeOracle->shouldSerialize(obj)) {     \
            XmlTagPrinter tagPrinter2(*this);                                  \
            if (writingP()) {                                                  \
              tagPrinter2.printNameMapItemOpenTag(iter.key(), obj);            \
            }                                                                  \
            trav(obj);                                                         \
          }                                                                    \
        }                                                                      \
      } else {                                                                 \
        for(PtrMap<char const, RANGETYPE>::Iter iter(*(OBJ));                  \
            !iter.isDone(); iter.adv()) {                                      \
          RANGETYPE *obj = iter.value();                                       \
          if (!serializeOracle || serializeOracle->shouldSerialize(obj)) {     \
            XmlTagPrinter tagPrinter2(*this);                                  \
            if (writingP()) {                                                  \
              tagPrinter2.printNameMapItemOpenTag(iter.key(), obj);            \
            }                                                                  \
            trav(obj);                                                         \
          }                                                                    \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  } while(0)

#define travStringRefMap(BASE, BASETYPE, FIELD, RANGETYPE) \
  travStringRefMap0(&((BASE)->FIELD), BASETYPE, FIELD, RANGETYPE)


#define trav(TARGET)                                                           \
  do {                                                                         \
    if (TARGET && (!serializeOracle || serializeOracle->shouldSerialize(TARGET))) { \
      toXml(TARGET);                                                           \
    }                                                                          \
  } while(0)

// NOTE: you must not wrap this one in a 'do {} while(0)': the dtor
// for the XmlCloseTagPrinter fires too early.

#define travListItem(TARGET)                                      \
  XmlTagPrinter tagPrinter(*this);                                \
  if (writingP()) {                                               \
    tagPrinter.printListItemOpenTag(TARGET);                      \
  }                                                               \
  IncDec depthManager(this->depth);                               \
  trav(TARGET)

#define travAST(TARGET)                                           \
  do {                                                            \
    if (TARGET) {                                                 \
      if (astVisitor) (TARGET)->traverse(*astVisitor);            \
    }                                                             \
  } while(0)


#endif // XML_WRITER_H
