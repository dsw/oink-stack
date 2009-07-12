// vdtllist.h            see license.txt for copyright and terms of use
// list of void*, with a pointer maintained to the last (tail)
// element, for constant-time append

#ifndef VDTLLIST_H
#define VDTLLIST_H

#include "voidlist.h"      // VoidList

// inherit privately so I can choose what to expose
class VoidTailList : private VoidList {
private:
  // by making this a friend, it should see VoidList as a
  // base class, and thus simply work
  // but it doesn't..
  //friend VoidListIter;

  friend class VoidTailListIter;
  friend class VoidTailListMutator;

protected:
  VoidNode *tail;       // (serf) last element of list, or NULL if list is empty
  VoidNode *getTop() const { return VoidList::getTop(); }

private:
  VoidTailList(VoidTailList const &obj);    // not allowed

  void adjustTail();

public:
  VoidTailList()                     { tail = NULL; }
  ~VoidTailList()                    {}

  // special ctor which steals the list and then deallocates the header
  VoidTailList(VoidTailList *src)    { tail = NULL; steal(src); }
  void steal(VoidTailList *src);     // deletes 'src'

  // this syntax just makes the implementation inherited from
  // 'VoidList' public, whereas it would default to private,
  // since it was inherited privately
  VoidList::count;

  // see voidlist.h for documentation of each of these functions
  VoidList::isEmpty;
  VoidList::isNotEmpty;
  VoidList::nth;
  VoidList::first;
  void *last() const                 { xassert(tail); return tail->data; }

  // insertion
  void prepend(void *newitem);
  void append(void *newitem);
  void appendAll(VoidTailList &tail);
  void insertAt(void *newitem, int index);
  void concat(VoidTailList &tail);

  // removal
  void *removeFirst();               // remove first, return data; must exist
  void *removeLast();
  void *removeAt(int index);
  bool removeIfPresent(void *item);
  void removeItem(void *item);
  void removeAll();

  // list-as-set: selectors
  VoidList::indexOf;
  VoidList::indexOfF;
  VoidList::contains;

  // list-as-set: mutators
  bool prependUnique(void *newitem);
  bool appendUnique(void *newitem);
  //void removeItem(void *item);
  //bool removeIfPresent(void *item);

  // debugging
  void selfCheck() const;
  VoidList::debugPrint;
};


// for traversing the list and modifying it
// NOTE: no list-modification fns should be called on 'list' while this
//       iterator exists, and only one such iterator should exist for
//       any given list
class VoidTailListMutator {
  friend class VoidTailListIter;

protected:
  VoidTailList &list; 	  // underlying list
  VoidNode *prev;         // (serf) previous node; NULL if at list's head
  VoidNode *current;      // (serf) node we're considered to be pointing at

public:
  VoidTailListMutator(VoidTailList &lst)   : list(lst) { reset(); }
  ~VoidTailListMutator()               {}

  void reset()                     { prev = NULL;  current = list.top; }

  // iterator copying; safe *only* until one of the mutators modifies
  // the list structure (by inserting or removing), at which time all
  // other iterators might be in limbo
  VoidTailListMutator(VoidTailListMutator const &obj)
    : list(obj.list), prev(obj.prev), current(obj.current) {}
  VoidTailListMutator& operator=(VoidTailListMutator const &obj);
    // requires that 'this' and 'obj' already refer to the same 'list'

  // iterator actions
  bool isDone() const              { return current == NULL; }
  void adv()                       { prev = current;  current = current->next; }
  void *data()                     { return current->data; }
  void *&dataRef()                 { return current->data; }

  // insertion
  void insertBefore(void *item);
    // 'item' becomes the new 'current', and the current 'current' is
    // pushed forward (so the next adv() will make it current again)

  void insertAfter(void *item);
    // 'item' becomes what we reach with the next adv();
    // isDone() must be false

  void append(void *item);
    // only valid while isDone() is true, it inserts 'item' at the end of
    // the list, and advances such that isDone() remains true; equivalent
    // to { xassert(isDone()); insertBefore(item); adv(); }

  // removal
  void *remove();
    // 'current' is removed from the list and returned, and whatever was
    // next becomes the new 'current'

  // debugging
  void selfCheck() const
    { xassert((prev->next == current  &&  current != list.top) ||
              (prev==NULL && current==list.top)); }
};

// copied from voidlist.h because g++ won't do what I want..
class VoidTailListIter {
protected:
  VoidNode *p;                        // (serf) current item

public:
  VoidTailListIter(VoidTailList const &list)  { reset(list); }
  VoidTailListIter()                          { p = NULL; }
  ~VoidTailListIter()                         {}

  void reset(VoidTailList const &list)        { p = list.getTop(); }

  // iterator copying; generally safe
  VoidTailListIter(VoidTailListIter const &obj)             { p = obj.p; }
  VoidTailListIter& operator=(VoidTailListIter const &obj)  { p = obj.p; return *this; }

  // but copying from a mutator is less safe; see above
  //VoidTailListIter(VoidListMutator &obj)      { p = obj.current; }

  // iterator actions
  bool isDone() const                         { return p == NULL; }
  void adv()                                  { p = p->next; }
  void *data() const                          { return p->data; }
  void *&dataRef()                            { return p->data; }

  // iterator mutation; use with caution
  void setDataLink(void *newData)             { p->data = newData; }
};



#endif // VDTLLIST_H
