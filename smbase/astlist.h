// astlist.h            see license.txt for copyright and terms of use
// owner list wrapper around VoidTailList
// name 'AST' is because the first application is in ASTs

#ifndef ASTLIST_H
#define ASTLIST_H

#include "vdtllist.h"     // VoidTailList

template <class T> class ASTListIter;
template <class T> class ASTListIterNC;
template <class T> class ASTListMutator;

// a list which owns the items in it (will deallocate them), and
// has constant-time access to the last element
template <class T>
class ASTList {
private:
  friend class ASTListIter<T>;
  friend class ASTListIterNC<T>;
  friend class ASTListMutator<T>;

protected:
  VoidTailList list;                    // list itself

private:
  ASTList(ASTList const &obj);          // not allowed
  void operator=(ASTList const &obj);   // not allowed

public:
  ASTList()                             : list() {}
  ~ASTList()                            { deleteAll(); }

  // ctor to make singleton list; often quite useful
  ASTList(T *elt)                       : list() { prepend(elt); }

  // stealing ctor; among other things, since &src->list is assumed to
  // point at 'src', this class can't have virtual functions;
  // these ctors delete 'src'
  ASTList(ASTList<T> *src)              : list(&src->list) {}
  void steal(ASTList<T> *src)           { deleteAll(); list.steal(&src->list); }

  // selectors
  int count() const                     { return list.count(); }
  bool isEmpty() const                  { return list.isEmpty(); }
  bool isNotEmpty() const               { return list.isNotEmpty(); }
  T *nth(int which)                     { return (T*)list.nth(which); }
  T const *nthC(int which) const        { return (T const*)list.nth(which); }
  T *first()                            { return (T*)list.first(); }
  T const *firstC() const               { return (T const*)list.first(); }
  T *last()                             { return (T*)list.last(); }
  T const *lastC() const                { return (T const*)list.last(); }

  // insertion
  void prepend(T *newitem)              { list.prepend(newitem); }
  void append(T *newitem)               { list.append(newitem); }
  void appendAll(ASTList<T> &tail)      { list.appendAll(tail.list); }
  void insertAt(T *newitem, int index)  { list.insertAt(newitem, index); }
  void concat(ASTList<T> &tail)         { list.concat(tail.list); }

  // removal
  T *removeFirst()                      { return (T*)list.removeFirst(); }
  T *removeLast()                       { return (T*)list.removeLast(); }
  T *removeAt(int index)                { return (T*)list.removeAt(index); }
  void removeItem(T *item)              { list.removeItem((void*)item); }
  bool removeIfPresent(T *item)         { return list.removeIfPresent((void*)item); }

  // this one is awkwardly named to remind the user that it's
  // contrary to the usual intent of this class
  void removeAll_dontDelete()           { return list.removeAll(); }

  // deletion
  void deleteFirst()                    { delete (T*)list.removeFirst(); }
  void deleteAll();
  void deleteItem(T* item)              { removeItem(item); delete item; }

  // list-as-set: selectors
  int indexOf(T const *item) const      { return list.indexOf((void*)item); }
  int indexOfF(T const *item) const     { return list.indexOfF((void*)item); }
  bool contains(T const *item) const    { return list.contains((void*)item); }

  // list-as-set: mutators
  bool prependUnique(T *newitem)        { return list.prependUnique(newitem); }
  bool appendUnique(T *newitem)         { return list.appendUnique(newitem); }

  // debugging: two additional invariants
  void selfCheck() const                { list.selfCheck(); }
};


template <class T>
void ASTList<T>::deleteAll()
{
  while (!list.isEmpty()) {
    deleteFirst();
  }
}


template <class T>
class ASTListIter {
protected:
  VoidTailListIter iter;      // underlying iterator

public:
  ASTListIter()                        {} // initially done
  ASTListIter(ASTList<T> const &list) : iter(list.list) {}
  ~ASTListIter()                       {}

  void reset(ASTList<T> const &list)   { iter.reset(list.list); }

  // iterator copying; generally safe
  ASTListIter(ASTListIter const &obj)             : iter(obj.iter) {}
  ASTListIter& operator=(ASTListIter const &obj)  { iter = obj.iter;  return *this; }

  // iterator actions
  bool isDone() const                   { return iter.isDone(); }
  void adv()                            { iter.adv(); }
  T const *data() const                 { return (T const*)iter.data(); }
};

#define FOREACH_ASTLIST(T, list, iter) \
  for(ASTListIter<T> iter(list); !iter.isDone(); iter.adv())


// version of the above, but for non-const-element traversal
template <class T>
class ASTListIterNC {
protected:
  VoidTailListIter iter;      // underlying iterator

public:
  ASTListIterNC()                      {} // initially done
  ASTListIterNC(ASTList<T> &list)      : iter(list.list) {}
  ~ASTListIterNC()                     {}

  void reset(ASTList<T> &list)         { iter.reset(list.list); }

  // iterator copying; generally safe
  ASTListIterNC(ASTListIterNC const &obj)             : iter(obj.iter) {}
  ASTListIterNC& operator=(ASTListIterNC const &obj)  { iter = obj.iter;  return *this; }

  // iterator actions
  bool isDone() const                   { return iter.isDone(); }
  void adv()                            { iter.adv(); }
  T *data() const                       { return (T*)iter.data(); }
  T *&dataRef()                         { return (T*&)iter.dataRef(); }

  // iterator mutation; use with caution
  void setDataLink(T *newData)          { iter.setDataLink((void*)newData); }
};

#define FOREACH_ASTLIST_NC(T, list, iter) \
  for(ASTListIterNC<T> iter(list); !iter.isDone(); iter.adv())


// this function is somewhat at odds with the nominal purpose
// of ASTLists, but I need it in a weird situation so ...
template <class T>
ASTList<T> *shallowCopy(ASTList<T> *src)
{
  ASTList<T> *ret = new ASTList<T>;
  FOREACH_ASTLIST_NC(T, *src, iter) {
    ret->append(iter.data());
  }
  return ret;
}

// for traversing the list and modifying it (nodes and/or structure)
// NOTE: no list-modification fns should be called on 'list' while this
//       iterator exists, and only one such iterator should exist for
//       any given list
template <class T>
class ASTListMutator {
  friend class ASTListIter<T>;

protected:
  VoidTailListMutator mut;       // underlying mutator

public:
  ASTListMutator(ASTList<T> &lst)     : mut(lst.list) { reset(); }
  ~ASTListMutator()                    {}

  void reset()                          { mut.reset(); }

  // iterator copying; safe *only* until one of the mutators modifies
  // the list structure (by inserting or removing), at which time all
  // other iterators might be in limbo
  ASTListMutator(ASTListMutator const &obj)             : mut(obj.mut) {}
  ASTListMutator& operator=(ASTListMutator const &obj)  { mut = obj.mut;  return *this; }
    // requires that 'this' and 'obj' already refer to the same 'list'

  // iterator actions
  bool isDone() const                   { return mut.isDone(); }
  void adv()                            { mut.adv(); }
  T *data()                             { return (T*)mut.data(); }
  T *&dataRef()                         { return (T*&)mut.dataRef(); }

  // insertion
  void insertBefore(T *item)            { mut.insertBefore((void*)item); }
    // 'item' becomes the new 'current', and the current 'current' is
    // pushed forward (so the next adv() will make it current again)

  void insertAfter(T *item)             { mut.insertAfter((void*)item); }
    // 'item' becomes what we reach with the next adv();
    // isDone() must be false

  void append(T *item)                  { mut.append((void*)item); }
    // only valid while isDone() is true, it inserts 'item' at the end of
    // the list, and advances such that isDone() remains true; equivalent
    // to { xassert(isDone()); insertBefore(item); adv(); }

  // removal
  T *remove()                           { return (T*)mut.remove(); }
    // 'current' is removed from the list and returned, and whatever was
    // next becomes the new 'current'

  void deleteIt()                       { delete (T*)mut.remove(); }
    // same as remove(), except item is deleted also

  // debugging
  void selfCheck() const                { mut.selfCheck(); }
};

#endif // ASTLIST_H
