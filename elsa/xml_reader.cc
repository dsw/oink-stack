// xml_reader.cc            see license.txt for copyright and terms of use

#include "xml_reader.h"         // this module
#include "exc.h"                // xBase
#include "taillist.h"
#include "sobjlist.h"
#include "objlist.h"

bool xmlDanglingPointersAllowed = true;

UnsatLink::UnsatLink(void *ptr0, char const *id0, int kind0, bool embedded0)
  : ptr(ptr0), id(id0), kind(kind0), embedded(embedded0)
{};


void XmlReader::setManager(XmlReaderManager *manager0) {
  xassert(!manager);
  manager = manager0;
}

void XmlReader::xmlUserFatalError(char const *msg) {
  manager->xmlUserFatalError(msg);
}

void XmlReaderManager::registerReader(XmlReader *reader) {
  xassert(reader);
  readers.append(reader);
  reader->setManager(this);
}

void XmlReaderManager::unregisterReader(XmlReader *reader) {
  xassert(reader);
  readers.deleteItem(reader);
}

void XmlReaderManager::reset() {
  // TODO: should this clear readers?
  lastNode = NULL;
  lastKind = 0;
  xassert(parseStack.isEmpty());
}

void XmlReaderManager::parseOneTopLevelTag() {
  // FIX: a do-while is always a bug
  do parseOneTagOrDatum();
  while(!atTopLevel());
}

// TODO: shorten this function
void XmlReaderManager::parseOneTagOrDatum() {
  static int count=0;
  ++count;

  // state: looking for a tag start
  if (lexer.haveSeenEof()) {
    xmlUserFatalError("unexpected EOF while looking for '<' of an open tag");
  }
  int start = lexer.getToken();
  //      printf("start:%s\n", lexer.tokenKindDesc(start).c_str());
  switch(start) {
  default:
    xmlUserFatalError("unexpected token while looking for '<' of an open tag");
    break;
  case XTOK_NAME:
    // this is raw data in between tags
    registerStringToken(parseStack.top().object, parseStack.top().kind, lexer.currentText());
    return;
    break;
  case 0:                     // eof
    return;
    break;
  case XTOK_LESSTHAN:
    break;                    // continue parsing
  }

  // state: read a tag name
  int tag = lexer.getToken();
  // construct the tag object on the stack
  void *topTemp = NULL;
  bool sawOpenTag = true;
  switch(tag) {
  default:
    topTemp = ctorNodeFromTag(tag);
    break;
  // Slash: start of a close tag
  case XTOK_SLASH:
    sawOpenTag = false;
    break;
  // ListItem: a list element
  case XTOK__List_Item:
    // TODO: avoid creating ListItems by just appending to the list above us
    // in the stack.
    topTemp = new ListItem();
    break;
  // NameMapItem: a name-map element
  case XTOK__NameMap_Item:
    topTemp = new NameMapItem();
    break;
  // MapItem: a map element
  case XTOK__Map_Item:
    topTemp = new MapItem();
    break;
//    // Special case the <__Link/> tag
//    case XTOK___Link:
//      topTemp = new UnsatBiLink();
//      break;
  }
  if (sawOpenTag) {
    // NOTE: even if it is a stand-alone tag that will not stay on the
    // stack, we still have to put it here as readAttributes()
    // attaches attributes to the node on the top of the stack (my
    // parser is some sort of stack machine).
    xassert(topTemp);
    // fprintf(stderr, "## parseOneTagOrDatum: sawOpenTag count=%d, node=%p\n", count, topTemp);
    ParseStackItem &psi = parseStack.pushAlt();
    psi.object = topTemp;
    psi.kind = tag;
    // ulinks must be new or had its contents moved to allUnsatLinks.
    xassert(psi.ulinks.isEmpty());

    // read the attributes
    bool sawContainerTag = readAttributes();

    // if it is a container tag, we just leave it on the stack
    if (sawContainerTag) {
      // state: we saw a container tag
      return;
    }

    // state: we saw a stand-alone tag.  FIX: I suppose I should
    // generalize this, but for now there is only one stand-alone tag
//      if (!tag == XTOK___Link) {
    xmlUserFatalError("illegal stand-alone tag");
//      }
//      UnsatBiLink *ulink = (UnsatBiLink*) topTemp;
//      if (!ulink->from) {
//        xmlUserFatalError("missing 'from' field on __Link tag");
//      }
//      if (!ulink->to) {
//        xmlUserFatalError("missing 'to' field on __Link tag");
//      }
//      unsatBiLinks.append(ulink);
//      // we don't need it on the stack anymore
//      nodeStack.pop();
//      kindStack.pop();            // FIX: delete the return?
    return;
  }

  // state: read a close tag name
  int closeTag = lexer.getToken();
  if (!closeTag) {
    xmlUserFatalError("unexpected file termination while looking for a close tag name");
  }
  if (parseStack.isEmpty()) {
    xmlUserFatalError("too many close tags");
  }
  lastNode = parseStack.top().object;
  lastKind = parseStack.top().kind;
  ASTList<UnsatLink> &ulinks = parseStack.top().ulinks;
  if (lastKind != closeTag) {
    xmlUserFatalError(stringc << "close tag " << lexer.tokenKindDescV(closeTag)
                      << " does not match open tag " << lexer.tokenKindDescV(lastKind));
  }

  // quarl 2006-06-01
  //    Hopefully, we've already satisfied most by now so this is usually
  //    empty.  Steal the rest of them onto the list of all unsat links.
  allUnsatLinks.concat(ulinks);
  xassert(ulinks.isEmpty());

  // use popMany() instead of pop() to avoid the operator= call
  parseStack.popMany(1);

  // state: read the '>' after a close tag
  int closeGreaterThan = lexer.getToken();
  switch(closeGreaterThan) {
  default: xmlUserFatalError("unexpected token while looking for '>' of a close tag");
  case 0: xmlUserFatalError("unexpected file termination while looking for '>' of a close tag");
  case XTOK_GREATERTHAN:
    break;
  }

  // quarl 2006-06-01
  //    Append to any list/map items.
  switch(lastKind) {
  default:
    // normal non-list tag; no further processing required.
    break;
  case XTOK__List_Item: appendListItem(); break;
  case XTOK__NameMap_Item: appendNameMapItem(); break;
  case XTOK__Map_Item: appendMapItem(); break;
  }

  // quarl 2006-06-01
  //    We had to prepend in appendListItem() due to lack of constant-time
  //    append, so now reverse.
  switch (kind2kindCat(lastKind)) {
  default:
    // nothing to do
    break;

  case KC_FakeList:
    reverseFakeList(*reinterpret_cast<void**>(lastNode), lastKind);
    break;

  case KC_SObjList:
    reinterpret_cast<SObjList<void>*>(lastNode)->reverse();
    break;

  case KC_ObjList:
    reinterpret_cast<ObjList<void>*>(lastNode)->reverse();
    break;

  case KC_ArrayStack:
    // TODO: consolidate
    break;
  }
}

// quarl 2006-06-01
//    FakeList strategy: since a prepend to a fakelist modifies the
//    pointer to the fakelist, we must have a place to store the
//    pointer.  Thus the item stored in the nodeStack is actually a
//    pointer to a FakeList*.
//
//    We expect the parent to point to us, so as soon as we
//    have an ID in readAttributes(), we'll deallocate this void*.

void XmlReaderManager::appendListItem() {
  // save the item tag
  ListItem *itemNode = (ListItem*)lastNode;

  // find the Node pointed to by the item; it should have been seen
  // by now
  if (itemNode->to.empty()) {
    xmlUserFatalError("no 'to' field for this _List_Item tag");
  }

  // TODO: optimization: 99% of the time, the pointedToItem was our xml-child
  // TODO: can just re-use UnsatLink to implement optimization
  void *pointedToItem = id2obj.queryif(itemNode->to);
  if (!pointedToItem) {
    if (!xmlDanglingPointersAllowed) xmlUserFatalError("no Node pointed to by _List_Item");
    return;
  }

  // what kind of thing is next on the stack?
  void *listNode = getTopNode0();
  if (!listNode) {
    xmlUserFatalError("a _List_Item tag not immediately under a List");
  }
  int listKind = parseStack.top().kind;
  KindCategory listKindCat = kind2kindCat(listKind);

  // quarl 2006-06-01
  //    For void*-based lists such as ASTList and ObjList, we can just
  //    reinterpret the list type and append, since it's just a type-safety
  //    wrapper with identical representation.
  //
  //    For others, namely FakeList and ArrayStack, we have no choice but to
  //    do a manual virtual dispatch on listKind.
  switch (listKindCat) {
  default:
    xmlUserFatalError("a _List_Item tag not immediately under a List");
    break;

  case KC_ASTList:
    reinterpret_cast<ASTList<void>*>(listNode)->append(pointedToItem);
    break;
  case KC_TailList:
    reinterpret_cast<TailList<void>*>(listNode)->append(pointedToItem);
    break;
  case KC_FakeList:
    // no constant append; reversed later
    prependToFakeList(*reinterpret_cast<void**>(listNode), pointedToItem, listKind);
    break;
  case KC_ObjList:
    // no constant append; reversed later
    reinterpret_cast<ObjList<void>*>(listNode)->prepend(pointedToItem);
    break;
  case KC_SObjList:
    // no constant append; reversed later
    reinterpret_cast<SObjList<void>*>(listNode)->prepend(pointedToItem);
    break;
  case KC_ArrayStack:
    appendToArrayStack(listNode, pointedToItem, listKind);
    break;
  }

  delete itemNode;
}

void XmlReaderManager::appendNameMapItem() {
  // save the name tag
  NameMapItem *nameNode = (NameMapItem*)lastNode;

  // find the Node pointed to by the item; it should have been seen
  // by now
  // TODO: factor and optimize as above
  void *pointedToItem = id2obj.queryif(nameNode->to);
  if (!pointedToItem) {
    if (!xmlDanglingPointersAllowed) xmlUserFatalError("no Node pointed to by _NameMap_Item");
  }

  void *mapNode = getTopNode0();
  if (!mapNode) {
    xmlUserFatalError("a _NameMap_Item tag not immediately under a Map");
  }
  int mapKind = parseStack.top().kind;
  KindCategory mapKindCat = kind2kindCat(mapKind);
  switch (mapKindCat) {
  default:
    xmlUserFatalError("a _NameMap_Item tag not immediately under a Map");
    break;

  case KC_StringRefMap: {
    char const *key = strTable(nameNode->from.c_str());
    StringRefMap<void>* map = reinterpret_cast <StringRefMap<void>*>(mapNode);
    if (map->get(key)) {
      xmlUserFatalError(stringc << "duplicate name " << key << " in map");
    }
    map->add(key, pointedToItem);
    break; }

  case KC_StringSObjDict: {
    string const &key = nameNode->from;
    StringSObjDict<void>* map = reinterpret_cast <StringSObjDict<void>*>(mapNode);
    if (map->isMapped(key)) {
      xmlUserFatalError(stringc << "duplicate name " << key << " in map");
    }
    map->add(key, pointedToItem);
    break; }
  }

  delete nameNode;
}

void XmlReaderManager::appendMapItem() {
  // save the name tag
  MapItem *nameNode = (MapItem*)lastNode;

  // find the Node-s pointed to by both the key and item; they
  // should have been seen by now
  void *pointedToKey = id2obj.queryif(nameNode->from);
  void *pointedToItem = id2obj.queryif(nameNode->to);
  if (!pointedToItem) {
    if (!xmlDanglingPointersAllowed) xmlUserFatalError("no Node pointed to by _Map_Item");
  }
  // TODO: check that pointedToKey not null?

  // what kind of thing is next on the stack?
  void *mapNode = getTopNode0();
  if (!mapNode) {
    xmlUserFatalError("a _NameMap_Item tag not immediately under a Map");
  }
  int mapKind = parseStack.top().kind;
  KindCategory mapKindCat = kind2kindCat(mapKind);

  switch (mapKindCat) {
  default:
    xmlUserFatalError("a _Map_Item tag not immediately under a Map");
    break;

  case KC_PtrMap: {
    PtrMap<void, void> *map = reinterpret_cast<PtrMap<void, void>*>(mapNode);
    if (map->get(pointedToKey)) {
      // there is no good way to print out the key
      xmlUserFatalError(stringc << "duplicate key in map");
    }
    map->add(pointedToKey, pointedToItem);
    break; }
  }
}

// state: read the attributes
bool XmlReaderManager::readAttributes() {
  size_t count = 0;
  while(1) {
    ++count;
    int attr = lexer.getToken();
    switch(attr) {
    default: break;             // go on; assume it is a legal attribute tag
    case 0: xmlUserFatalError("unexpected file termination while looking for an attribute name");
    case XTOK_GREATERTHAN:
      return true;              // container tag
    case XTOK_SLASH:
      attr = lexer.getToken();  // eat the '>' token
      if (attr!=XTOK_GREATERTHAN) {
        xmlUserFatalError("expected '>' after '/' that terminates a stand-alone tag");
      }
      return false;             // non-container tag
    }

    int eq = lexer.getToken();
    switch(eq) {
    default: xmlUserFatalError("unexpected token while looking for an '='");
    case 0: xmlUserFatalError("unexpected file termination while looking for an '='");
    case XTOK_EQUAL:
      break;                    // go on
    }

    int value = lexer.getToken();
    switch(value) {
    default: xmlUserFatalError("unexpected token while looking for an attribute value");
    case 0: xmlUserFatalError("unexpected file termination while looking for an attribute value");
    case XTOK_INT_LITERAL:
      // get it out of yytext below
      break;
    case XTOK_STRING_LITERAL:
      // Get it out of yytext below.  Note that this string has already been
      // dequoted/unescaped in the lexer!
      break;                    // go on
    }

    // register the attribute
    xassert(parseStack.isNotEmpty());
    // special case the '_id' attribute
    if (attr == XTOK_DOT_ID) {
      // the map makes a copy of this string (if it needs it)
      const char *id0 = lexer.currentText();
      // DEBUG1
      // cout << "## readAttributes: _id=" << id0 << endl;

      // quarl 2006-06-01
      //    Important: record the kind before we call satisfyUnsatLink,
      //    because it needs to know what kind to upcast to.
      if (recordKind(parseStack.top().kind)) {
        id2kind.add(id0, parseStack.top().kind);
      }

#if 1
      // quarl 2006-06-01: preemptive link satisfaction
      //    See if the parent XML node has an unsatisfied link pointing to
      //    this one -- the common case -- so that we can get rid of the
      //    unsatLink, saving us from having to do a more work later.  If it's
      //    an embedded object, even better: just point to the target.

      // TODO: when parsing the tag, peek to see if it's an upcoming _id for
      // an embedded object, and if so, don't even construct one!

      UnsatLink *ulink = getUnsatLink(id0);
      if (ulink) {
        if (count == 1 && ulink->embedded)
        {
          // quarl 2006-06-01 if this is the first attribute then we don't
          // need to copy anything; just delete the temporary.
          void *obj = parseStack.top().object;
          xassert(obj);
          deleteObj(obj, ulink->kind);
        } else {
          satisfyUnsatLink(ulink, parseStack.top().object);
        }

        if (ulink->embedded) {
          // now we point directly to the place we want to write to.
          parseStack.top().object = ulink->ptr;
        }

        // yay, got rid of a ulink!
        delete ulink;
      }
#endif

      if (id2obj.isMapped(id0)) {
        xmlUserFatalError(stringc << "this _id is taken: " << id0);
      }
      id2obj.add(id0, parseStack.top().object);
    }
    // special case the _List_Item node and its one attribute
    else if (parseStack.top().kind == XTOK__List_Item) {
      switch(attr) {
      default:
        xmlUserFatalError("illegal attribute for _List_Item");
        break;
      case XTOK_item:
        static_cast<ListItem*>(parseStack.top().object)->to =
          lexer.currentText();
        break;
      }
    }
    // special case the _NameMap_Item node and its one attribute
    else if (parseStack.top().kind == XTOK__NameMap_Item) {
      switch(attr) {
      default:
        xmlUserFatalError("illegal attribute for _NameMap_Item");
        break;
      case XTOK_name:
        static_cast<NameMapItem*>(parseStack.top().object)->from =
          lexer.currentText();
        break;
      case XTOK_item:
        static_cast<NameMapItem*>(parseStack.top().object)->to =
          lexer.currentText();
        break;
      }
    }
    // special case the _Map_Item node and its one attribute
    else if (parseStack.top().kind == XTOK__Map_Item) {
      switch(attr) {
      default:
        xmlUserFatalError("illegal attribute for _Map_Item");
        break;
      case XTOK_key:
        static_cast<MapItem*>(parseStack.top().object)->from =
          lexer.currentText();
        break;
      case XTOK_item:
        static_cast<MapItem*>(parseStack.top().object)->to =
          lexer.currentText();
        break;
      }
    }
//      // special case the __Link node and its attributes
//      else if (kindStack.top() == XTOK___Link) {
//        switch(attr) {
//        default:
//          xmlUserFatalError("illegal attribute for __Link");
//          break;
//        case XTOK_from:
//          static_cast<UnsatBiLink*>(nodeStack.top())->from =
//            lexer.currentText();
//          break;
//        case XTOK_to:
//          static_cast<UnsatBiLink*>(nodeStack.top())->to =
//            lexer.currentText();
//          break;
//        }
//      }
    // not a built-in attribute or tag
    else {
      registerAttribute(parseStack.top().object, parseStack.top().kind, attr, lexer.currentText());
    }
  }
  if (!parseStack.isEmpty()) {
    xmlUserFatalError("missing closing tags at eof");
  }
  // stacks should not be out of sync
  xassert(parseStack.isEmpty());
}

void XmlReaderManager::kind2kindCat(int kind, KindCategory *kindCat) {
  xassert(kind != -1);          // this means you shouldn't be asking

  switch(kind) {
  default:
    // fallthrough the switch
    break;

  // special list element _List_Item
  case XTOK__List_Item:
    *kindCat = KC_Item;
    return;
    break;

  // special map element _NameMap_Item
  case XTOK__NameMap_Item:
    *kindCat = KC_Name;
    return;
    break;
  }

  // try each registered reader
  FOREACH_ASTLIST_NC(XmlReader, readers, iter) {
    if (iter.data()->kind2kindCat(kind, kindCat)) {
      return;
    }
  }

  xfailure(stringc << "no kind category registered for kind "
           << lexer.tokenKindDescV(kind));
}

void *XmlReaderManager::ctorNodeFromTag(int tag) {
  xassert(tag != -1);          // this means you shouldn't be asking

  // try each registered reader
  FOREACH_ASTLIST_NC(XmlReader, readers, iter) {
    XmlReader *r = iter.data();
    void *node = r->ctorNodeFromTag(tag);
    if (node) {
      xassert(node != (void*) 0xBADCAFE);
      return node;
    }
  }

  xmlUserFatalError(stringc
                    << "no ctor registered for tag " << lexer.tokenDesc(tag));
}

void XmlReaderManager::registerAttribute
  (void *target, int kind, int attr, char const *yytext0) {
  xassert(kind != -1);          // this means you shouldn't be asking

  // try each registered reader
  FOREACH_ASTLIST_NC(XmlReader, readers, iter) {
    if (iter.data()->registerAttribute(target, kind, attr, yytext0)) {
      return;
    }
  }

  xfailure("no handler registered for this tag and attribute");
}

void XmlReaderManager::registerStringToken(void *target, int kind, char const *yytext0) {
  xassert(kind != -1);          // this means you shouldn't be asking

  // try each registered reader
  FOREACH_ASTLIST_NC(XmlReader, readers, iter) {
    if (iter.data()->registerStringToken(target, kind, yytext0)) {
      return;
    }
  }

  xfailure("no raw data handler registered for this tag");
}

void XmlReaderManager::xmlUserFatalError(char const *msg) {
  stringBuilder msg0;
  if (inputFname) {
    msg0 << inputFname << ':';
  }
  msg0 << lexer.linenumber << ": " << msg;
  THROW(xBase(msg0));
  xfailure("should not get here");
}

// quarl 2006-06-01:
//    We now maintain a list of UnsatLinks for each level of the parse stack;
//    when we pop them we concat them to the global list.  The per-level unsat
//    links allow us to quickly scan the parent level's unsat links and
//    satisfy them now, yielding a big performance gain.  This is also
//    necessary for deserializing FakeLists directly into the target location.
//
//    If a link is found, return the UnsatLink, which the caller must delete.
//    Otherwise, return NULL.

UnsatLink *XmlReaderManager::getUnsatLink(char const *id0)
{
  if (parseStack.length() <= 1) {
    return NULL;
  }

  ASTList<UnsatLink> &parentUnsatLinks = parseStack.nth(1).ulinks;
  for (ASTListMutator<UnsatLink> mut(parentUnsatLinks); !mut.isDone(); mut.adv()) {
    UnsatLink *ulink = mut.data();
    if (streq(ulink->id, id0)) {
      // remove this ulink; caller deletes
      mut.remove();
      return ulink;
    }
  }
  return NULL;
}

void XmlReaderManager::addUnsatLink(UnsatLink *u)
{
  // I OWNZ U
  // fprintf(stderr, "## addUnsatLink: u=%p, u->id=%s, u->embedded=%d\n",
  //         u, u->id.c_str(), u->embedded);
  parseStack.top().ulinks.append(u);
}


// quarl 2006-05-31:
//    UNFORTUNATELY, the order in which we satisfy links matters when embedded
//    objects are involved.  Embedded objects must be satisfied last, and if
//    there are nested embedding, then the outermost object must be satisfied
//    last.  The order we use now (lists, nodes without embedding, nodes with
//    embedding) works for now, but the whole thing is brittle.

// TODO: make separate lists for embedded and non-embedded ulinks

void XmlReaderManager::satisfyLinks() {
  // satisfyLinks_Lists();
  satisfyLinks_Nodes();
  // satisfyLinks_Maps();
//    satisfyLinks_Bidirectional();
}

#include "xmlhelp.h"
static inline
bool inputXmlPointerIsNull(char const * s_id) {
  return streq(s_id, "(null)");
}

void XmlReaderManager::satisfyLinks_Nodes() {
  // printf("## id2obj.size() = %d, id2kind.size() = %d\n",
  //        id2obj.size(), id2kind.size());

  // satisfy non-embedded objects first, then embedded objects (see 2006-05-31
  // comment above)
  satisfyLinks_Nodes_1(false);
  satisfyLinks_Nodes_1(true);

  // remove the links
  allUnsatLinks.deleteAll();
}

void XmlReaderManager::satisfyLinks_Nodes_1(bool processEmbedded) {
  FOREACH_ASTLIST(UnsatLink, allUnsatLinks, iter) {
    UnsatLink const *ulink = iter.data();

    if (ulink->embedded != processEmbedded)
      continue;

    // fprintf(stderr, "## satisfyLinks_Nodes: processing link id=%s\n", ulink->id.c_str());
    void *obj;
    if (inputXmlPointerIsNull(ulink->id.c_str())) {
      // Note: as of 2006-05-30, we no longer serialize "(null)" pointers, so
      // when deserializing data that we serialized, we just get a dangling
      // pointer which defaults to NULL.
      obj = NULL;
    } else {
      obj = id2obj.queryif(ulink->id);
      if (obj == NULL) {
        if (!xmlDanglingPointersAllowed) {
          xmlUserFatalError(stringc << "unsatisfied node link: " << ulink->id);
        }
      }
    }

    satisfyUnsatLink(ulink, obj);
  }
}

void XmlReaderManager::satisfyUnsatLink(UnsatLink const *ulink, void *obj)
{
  if (ulink->embedded) {
    // I can assume that the kind of the object that was
    // de-serialized is the same as the target because it was
    // embedded and there is no chance for a reference/referent
    // type mismatch.
    callOpAssignToEmbeddedObj(obj, ulink->kind, ulink->ptr);
    // FIX: we should now delete obj; in fact, this should be done
    // by callOpAssignToEmbeddedObj() since it knows the type of
    // the object which is necessary to delete it of course.  I'll
    // leave this for an optimization pass which we will do later
    // to handle many of these things.
    deleteObj(obj, ulink->kind);

    // we should not use this obj anymore
    id2obj.add(ulink->id, (void*) 0xBADCAFE);
  } else {
    if (int kind = id2kind.queryif(ulink->id)) {
      *( (void**)(ulink->ptr) ) = upcastToWantedType(obj, kind, ulink->kind);
    } else {
      // no kind was registered for the object and therefore no
      // upcasting is required and there is no decision to make; so
      // just do the straight pointer assignment
      *( (void**) (ulink->ptr) ) = obj;
    }
  }
}

//  void XmlReaderManager::satisfyLinks_Bidirectional() {
//    FOREACH_ASTLIST(UnsatBiLink, unsatBiLinks, iter) {
//      UnsatBiLink const *ulink = iter.data();
//      void **from = (void**)id2obj.queryif(ulink->from);
//      // NOTE: these are different from unidirectional links: you really
//      // shouldn't make one unless both parties can be found.
//      if (!from) {
//        xmlUserFatalError("Unsatisfied bidirectional link: 'from' not found");
//      }
//      void *to = id2obj.queryif(ulink->to);
//      if (!to) {
//        xmlUserFatalError("Unsatisfied bidirectional link: 'to' not found");
//      }
//      *(from) = to;
//    }
//  }

bool XmlReaderManager::recordKind(int kind) {
  FOREACH_ASTLIST_NC(XmlReader, readers, iter) {
    bool answer;
    XmlReader *r = iter.data();
    if (r->recordKind(kind, answer)) {
      return answer;
    }
  }
  THROW(xBase(stringc << "no way to decide if kind should be recorded"));
}

// Do "*target = *obj" for the right type.
// It's okay to destroy the contents of target since it's an embedded object,
// so it can only be used once.
// target is deleted later in deleteObj().
void XmlReaderManager::callOpAssignToEmbeddedObj(void *obj, int kind, void *target) {
  xassert(obj != target);
  KindCategory kindcat = kind2kindCat(kind);

  // quarl 2006-06-01
  //    For list items, steal obj's list into the target.  This avoids doing a
  //    needless O(N) copy (which we aren't allowed to do anyway for owning
  //    lists).
  // quarl 2006-06-02
  //    If any of these target->isEmpty() assertions are failing, something is
  //    wrong: since these lists are embedded (since we're in
  //    callOpAssignToEmbeddedObj()), it should not be possible to have two
  //    objects referring to the same one.  The most likely error is that the
  //    data member is actually a pointer to a list rather than an embedded
  //    list.
  switch (kindcat) {
  default: break;

  case KC_ASTList:
    if (!obj) return;
    xassert(reinterpret_cast<ASTList<void>*>(target)->isEmpty());
    reinterpret_cast<ASTList<void>*>(target)->concat(
      *reinterpret_cast<ASTList<void>*>(obj));
    return;
  case KC_TailList:
    if (!obj) return;
    xassert(reinterpret_cast<TailList<void>*>(target)->isEmpty());
    reinterpret_cast<TailList<void>*>(target)->concat(
      *reinterpret_cast<TailList<void>*>(obj));
    return;

  case KC_FakeList: {
    if (!obj) return;
    // for fake lists, it's just a pointer location, so copy the pointer.
    *((void**) target) = *((void**) obj);
    return; }

  case KC_ObjList:
    if (!obj) return;
    xassert(reinterpret_cast<ObjList<void>*>(target)->isEmpty());
    reinterpret_cast<ObjList<void>*>(target)->concat(
      *reinterpret_cast<ObjList<void>*>(obj));
    return;
  case KC_SObjList:
    if (!obj) return;
    xassert(reinterpret_cast<SObjList<void>*>(target)->isEmpty() && "61010461-dc93-4312-bca7-219392176c22");
    reinterpret_cast<SObjList<void>*>(target)->concat(
      *reinterpret_cast<SObjList<void>*>(obj));
    return;
  case KC_ArrayStack:
    // can't do it without the type, so have to use dispatch
    break;

  case KC_StringRefMap:
    xfailure("should have been preemptively satisfied");
    break;

  case KC_StringSObjDict:
    xfailure("should have been preemptively satisfied");
    break;

  }

  FOREACH_ASTLIST_NC(XmlReader, readers, iter) {
    XmlReader *r = iter.data();
    if (r->callOpAssignToEmbeddedObj(obj, kind, target)) {
      return;
    }
  }
  THROW(xBase(stringc << "no way to call op assign"));
}

void XmlReaderManager::deleteObj(void *obj, int kind)
{
  KindCategory kindcat = kind2kindCat(kind);
  switch (kindcat) {
  default: break;

  case KC_FakeList: {
    // the allocated object is actually a void*
    void **ptr = (void**) obj;
    delete ptr;
    return; }
  }

  // TODO
}


void *XmlReaderManager::upcastToWantedType(void *obj, int kind, int targetKind) {
  FOREACH_ASTLIST_NC(XmlReader, readers, iter) {
    void *target = (void*) 0xBADCAFE;
    XmlReader *r = iter.data();
    if (r->upcastToWantedType(obj, kind, &target, targetKind)) {
      xassert(target != (void*) 0xBADCAFE);
      return target;
    }
  }
  THROW(xBase(stringc << "no way to upcast"));
}

void XmlReaderManager::prependToFakeList(void *&list, void *obj, int listKind) {
  FOREACH_ASTLIST_NC(XmlReader, readers, iter) {
    XmlReader *r = iter.data();
    if (r->prependToFakeList(list, obj, listKind)) {
      return;
    }
  }
  THROW(xBase(stringc << "no prepender for FakeList type"));
}

void XmlReaderManager::reverseFakeList(void *&list, int listKind) {
  FOREACH_ASTLIST_NC(XmlReader, readers, iter) {
    XmlReader *r = iter.data();
    if (r->reverseFakeList(list, listKind)) {
      return;
    }
  }
  THROW(xBase(stringc << "no reverser for FakeList type"));
}

void XmlReaderManager::appendToArrayStack(void *list, void *obj, int listKind) {
  FOREACH_ASTLIST_NC(XmlReader, readers, iter) {
    XmlReader *r = iter.data();
    if (r->appendToArrayStack(list, obj, listKind)) {
      return;
    }
  }
  THROW(xBase(stringc << "no appender for ArrayStack type"));
}
