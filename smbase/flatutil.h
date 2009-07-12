// flatutil.h
// additional utilities layered upon 'flatten' interface

#ifndef FLATUTIL_H
#define FLATUTIL_H

#include "flatten.h"         // underlying module
#include "objlist.h"         // ObjList
#include "xassert.h"         // xassert


// Nominal way to create a 'T' object for unflattening; but you
// can overload to do things differently where needed.
template <class T>
T *createForUnflat(Flatten &flat)
{
  return new T(flat);
}

// Nominal way to flatten.  Again, overload to override.
template <class T>
void xfer(Flatten &flat, T &t)
{
  t.xfer(flat);
}


// Transfer an owner list.  First we transfer the number of elements,
// then each element in sequence.  If 'noteOwner' is true, the
// pointers are noted so that it is possible to later transfer serf
// aliases.
template <class T>
void xferObjList(Flatten &flat, ObjList<T> &list, bool noteOwner = false)
{
  if (flat.writing()) {
    flat.writeInt(list.count());
    FOREACH_OBJLIST_NC(T, list, iter) {
      T *t = iter.data();
      xfer(flat, *t);
      if (noteOwner) {
        flat.noteOwner(t);
      }
    }
  }
  else {
    list.deleteAll();
    int ct = flat.readInt();
    while (ct--) {
      T *t = createForUnflat<T>(flat);
      xfer(flat, *t);
      if (noteOwner) {
        flat.noteOwner(t);
      }
      list.prepend(t);
    }
    list.reverse();
  }
}


// Cast from one scalar to another, asserting representability of
// the value in the target type.
template <class DEST, class SRC>
inline DEST value_cast(SRC s)
{
  DEST d = (DEST)s;      // convert to DEST
  xassert((SRC)d == s);  // convert back, assert equal
  return d;
}


// Transfer an enum value.  This is safer than just casting to int
// reference, since it works when 'int' is not the same size as the
// enum.
template <class E>
void xferEnum(Flatten &flat, E &e)
{
  if (flat.writing()) {
    flat.writeInt(value_cast<int>(e));
  }
  else {
    e = value_cast<E>(flat.readInt());
  }
}


#endif // FLATUTIL_H
