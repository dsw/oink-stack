// xml_reader.h            see license.txt for copyright and terms of use

// Support for XML de-serialization

// FIX: this module should eventually go into the ast repository

#ifndef XML_READER_H
#define XML_READER_H

#include "id_obj_dict.h"        // IdSObjDict
#include "strintdict.h"         // StringIntDict
#include "bitwise_array.h"      // BitwiseArrayStack
#include "astlist.h"            // ASTList
#include "strtable.h"           // StringRef
#include "strmap.h"             // StringRefMap
#include "xml_lexer.h"          // XmlLexer
#include "fakelist.h"

class StringTable;

// forwards in this file
class XmlReaderManager;


// if on the deserialization code will tolerate dangling pointers: ids
// to objects that themselves were not serialized
extern bool xmlDanglingPointersAllowed;


// from Xml for enums
#define READENUM(X) else if (streq(str, #X)) out = (X)
#define READFLAG(X) else if (streq(token, #X)) out |= (X)

#define ul(FIELD, KIND) \
  manager->addUnsatLink \
    (new UnsatLink((void*) &(obj->FIELD), \
                   strValue, \
                   (KIND), \
                   false))

#define ulEmbed(FIELD, KIND) \
  manager->addUnsatLink \
    (new UnsatLink((void*) &(obj->FIELD), \
                   strValue, \
                   (KIND), \
                   true))

// there are 3 categories of kinds of Tags
enum KindCategory {
  // normal node
  KC_Node,

  // list
  KC_ASTList,
  KC_TailList,
  KC_FakeList,
  KC_ObjList,
  KC_SObjList,
  KC_ArrayStack,
  KC_Item,                      // an item entry in a list

  // name map
  KC_StringRefMap,
  KC_StringSObjDict,
  KC_Name,                      // a name entry in a name map

  // map
  KC_PtrMap,
  KC_Key,                       // a key entry in a name map
};

// the <_List_Item> </_List_Item> tag is parsed into this class to
// hold the name while the value contained by it is being parsed.
// Then it is deleted.
struct ListItem {
  string to;
};

// the <_NameMap_Item> </_NameMap_Item> tag is parsed into this class
// to hold the name while the value contained by it is being parsed.
// Then it is deleted.
struct NameMapItem {
  string from;
  string to;
};

// the <_Map_Item> </_Map_Item> tag is parsed into this class
// to hold the name while the value contained by it is being parsed.
// Then it is deleted.
struct MapItem {
  string from;
  string to;
};

// datastructures for dealing with unsatisified links; FIX: we can
// do the in-place recording of a lot of these unsatisified links
// (not the ast links)

// TODO: make ptr a 'void **'

// An unsatisfied link from an object A to another B
struct UnsatLink {
  void *ptr;                    // a ptr to a member of A that should point to B (unless embedded)
  string id;                    // id of B
  int kind;                     // type of B (roll-our-own-RTTI)
  bool embedded;                // B embedded into A or pointed at?
  UnsatLink(void *ptr0, char const *id0, int kind0, bool embedded0);
};

//  // datastructures for dealing with unsatisified links where neither
//  // party wants to know about the other
//  struct UnsatBiLink {
//    char const *from;
//    char const *to;
//    UnsatBiLink() : from(NULL), to(NULL) {}
//  };

// TODO: use an array of vtables for each tag, then we don't need this manual
// dispatch stuff.

// struct XmlKind {
//   virtual void *construct() = 0;
//   virtual bool upcastTo(void *obj, void *&target, XmlKind const &targetKind)
//   { xfailure("cannot upcast from this type"); }
//   virtual bool prependToFakeList(void *&list, void *obj, int objKind) { return false; }
//   virtual bool appendToArrayStack(void *list, void *obj, int objKind) { return false; }
//   class

// };

// template <class T>
// struct XmlKindT : XmlKind {
//   virtual void *construct() { return new T(); }

// };

// A subclass fills-in the methods that need to know about individual
// tags.
class XmlReader {
  protected:
  XmlReaderManager *manager;

  public:
  XmlReader() : manager(NULL) {}
  virtual ~XmlReader() {}

  public:
  void setManager(XmlReaderManager *manager0);

  protected:
  void xmlUserFatalError(char const *msg) NORETURN;

  // **** virtual API
  public:

  // Parse a tag: construct a node for a tag
  virtual void *ctorNodeFromTag(int tag) = 0;

  // Parse an attribute: register an attribute into the current node
  virtual bool registerAttribute(void *target, int kind, int attr, char const *yytext0) = 0;

  // Parse raw data: register some raw data into the current node
  virtual bool registerStringToken(void *target, int kind, char const *yytext0) = 0;

  // implement an eq-relation on tag kinds by mapping a tag kind to a
  // category
  virtual bool kind2kindCat(int kind, KindCategory *kindCat) = 0;

  // **** Generic Convert

  // note: return whether we know the answer, not the answer which
  // happens to also be a bool
  virtual bool recordKind(int kind, bool& answer) = 0;

  // cast a pointer to the pointer type we need it to be; this is only
  // needed because of multiple inheritance
  virtual bool callOpAssignToEmbeddedObj(void *obj, int kind, void *target) = 0;
  virtual bool upcastToWantedType(void *obj, int kind, void **target, int targetKind) = 0;
  virtual bool prependToFakeList(void *&list, void *obj, int listKind) { return false; }
  virtual bool reverseFakeList(void *&list, int listKind) { return false; }
  virtual bool appendToArrayStack(void *list, void *obj, int listKind) { return false; }

protected:

  // TODO: use pushAlt() so we avoid extra object creation
  template <class T>
  static
  void appendToArrayStack0(void *arrayStack, void *obj)
  { static_cast<ArrayStack<T>*>(arrayStack)->push(* static_cast<T*>(obj));
    delete static_cast<T*>(obj); }

  template <class T>
  static
  void prependToFakeList0(void *&list, void *obj)
  { list = static_cast<FakeList<T>*>(list)->prepend(static_cast<T*>(obj)); }

  template <class T>
  static
  void reverseFakeList0(void *&list)
  { list = static_cast<FakeList<T>*>(list)->reverse(); }

};

// XmlReader-s register themselves with the Manager which tries them
// one at a time while handling incoming xml tags.
class XmlReaderManager {
  // the readers we are managing
  ASTList<XmlReader> readers;

  // **** Parsing
  public:
  char const *inputFname;       // just for error messages
  XmlLexer &lexer;              // a lexer on a stream already opened from the file
  // TODO: partition StringTables as much as possible
  StringTable &strTable;        // for canonicalizing the StringRef's in the input file

  private:
  // the node (and its kind) for the last closing tag we saw; useful
  // for extracting the top of the tree
  void *lastNode;
  int lastKind;

  // parsing stack
  struct ParseStackItem {
    void *object;
    int kind;
    ASTList<UnsatLink> ulinks;
  };

  BitwiseArrayStack<ParseStackItem> parseStack;

  // **** Satisfying links

protected:
  // Since AST nodes are embedded, we have to put this on to a
  // different list than the ususal pointer unsatisfied links.
  ASTList<UnsatLink> allUnsatLinks;
//    ASTList<UnsatBiLink> unsatBiLinks;

  // map object ids to the actual object
  IdSObjDict id2obj;
  // StringSObjDict<void> id2obj;

  // map object ids to their kind ONLY IF there is a non-trivial
  // upcast to make at the link satisfaction point
  StringIntDict id2kind;
  // StringSObjDict<int> id2kind;

  public:
  XmlReaderManager(XmlLexer &lexer0, StringTable &strTable0)
    : inputFname(NULL)
    , lexer(lexer0)
    , strTable(strTable0)
    , lastNode(NULL)            // also done in reset()
    , lastKind(0)               // also done in reset()
  {
    reset();
  }
  virtual ~XmlReaderManager() {
    // We no longer need to do the following since we *own* the readers:
    //readers.removeAll_dontDelete();
  }

  // **** initialization
  public:
  // NOTE: XmlReaderManager owns the readers that have been registered with
  // it. It deallocates them on destruction and also upon unregisterReader.
  void registerReader(XmlReader *reader);

  // unregister a reader (deallocating it in the process); this is only needed
  // if you no longer want to use a reader, otherwise the reader is
  // deallocated automatically on destruction.
  void unregisterReader(XmlReader *reader);

  void reset();

  // **** parsing
  public:
  void parseOneTopLevelTag();

  private:
  void parseOneTagOrDatum();
  bool readAttributes();

  void appendListItem();
  void appendNameMapItem();
  void appendMapItem();

public:
  void *getNodeById(char const *id) { return id2obj.queryif(id); }

  // add an UnsatLink to the current stack node's unsatLinks.  Owns ulinks.
  void addUnsatLink(UnsatLink *ulink);

private:
  void *getTopNode0() { return parseStack.isEmpty() ? NULL : parseStack.top().object; }

  // try to find a link from topmost list of unsatLinks.
  UnsatLink *getUnsatLink(char const *id0);

  // satisfy a single UnsatLink
  void satisfyUnsatLink(UnsatLink const *ulink, void *obj);

  // disjunctive dispatch to the list of readers
  void kind2kindCat(int kind, KindCategory *kindCat);
  KindCategory kind2kindCat(int kind) { KindCategory kindCat; kind2kindCat(kind, &kindCat); return kindCat; }
  void *ctorNodeFromTag(int tag);
  void registerAttribute(void *target, int kind, int attr, char const *yytext0);
  void registerStringToken(void *target, int kind, char const *yytext0);

  void append2List(void *list, int listKind, void *datum);
  void insertIntoNameMap(void *map, int mapKind, StringRef name, void *datum);
  void insertIntoMap(void *map0, int mapKind, void *key, void *item);

  // **** parsing result
  public:
  // report an error to the user with source location information
  void xmlUserFatalError(char const *msg) NORETURN;
  // are we at the top level during parsing?
  bool atTopLevel() {return parseStack.isEmpty();}
  // return the top of the stack: the one tag that was parsed
  void *getLastNode() {return lastNode;}
  int getLastKind() {return lastKind;}
  // peek at nth item (linear time)
  void *getNthNode(int which) { return parseStack.nth(which).object; }
  int getNthKind(int which) { return parseStack.nth(which).kind; }

  // peek, with assertion for kind
  void *getNthNode(int which, int kind)
  { xassert(getNthKind(which) == kind); return getNthNode(which); }

  // **** satisfying links
  public:
  void satisfyLinks();

  private:
  void satisfyLinks_Nodes();
  void satisfyLinks_Nodes_1(bool processEmbedded);
  void satisfyLinks_Lists();
  void satisfyLinks_Maps();
//    void satisfyLinks_Bidirectional();

  public:
  bool recordKind(int kind);

  private:
  // convert nodes
  void callOpAssignToEmbeddedObj(void *obj, int kind, void *target);
  void deleteObj(void *obj, int kind);
  void *upcastToWantedType(void *obj, int kind, int targetKind);
  void prependToFakeList(void *&list, void *obj, int listKind);
  void reverseFakeList(void *&list, int listKind);
  void appendToArrayStack(void *list, void *obj, int listKind);
};

#endif // XML_READER_H
