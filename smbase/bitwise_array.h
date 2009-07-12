// bitwise_array.h            see license.txt for copyright and terms of use
// some array classes that use bitwise copying when expanding

#ifndef BITWISE_ARRAY_H
#define BITWISE_ARRAY_H

#include "xassert.h"      // xassert
#include <stdlib.h>       // qsort
#include <string.h>       // memcpy
#include <new.h>          // new


// ------------------ BitwiseGrowArray --------------------
// This class implements an array of T's; it automatically expands
// when 'ensureAtLeast' or 'ensureIndexDoubler' is used; it does not
// automatically contract.  All accesses are bounds-checked.
//
// class T must have:
//   T::T();           // default ctor for making arrays
//   T::~T();          // dtor for when old array is cleared
//
// to use copyFrom, T must have:
//   T::operator=(T const&);
//
// quarl 2006-06-01
//   BitwiseGrowArray uses malloc/free and manually calls T() and ~T(), so that we
//   don't need to have T::operator=(T&); this is also faster since we only do
//   a shallow copy.

template <class T>
class BitwiseGrowArray {
private:     // data
  T *arr;                 // underlying array; NULL if sz==0
  int sz;                 // # allocated entries in 'arr'

private:     // funcs
  void bc(int i) const    // bounds-check an index
    { xassert((unsigned)i < (unsigned)sz); }
  void eidLoop(int index);

  // make 'this' equal to 'obj'
  void copyFrom(BitwiseGrowArray<T> const &obj) {
    setSize(obj.size());     // not terribly efficient, oh well
    copyFrom_limit(obj, sz);
  }

protected:   // funcs
  void copyFrom_limit(BitwiseGrowArray<T> const &obj, int limit);

private:     // disallowed
  void operator=(BitwiseGrowArray&);
  void operator==(BitwiseGrowArray&);

public:      // funcs
  BitwiseGrowArray(int initSz);
  BitwiseGrowArray(BitwiseGrowArray const &obj) : arr(0), sz(0) { copyFrom(obj); }
  ~BitwiseGrowArray();

  BitwiseGrowArray& operator=(BitwiseGrowArray const &obj) { copyFrom(obj); return *this; }

  // allocated space
  int size() const { return sz; }

  // element access
  T const& operator[] (int i) const   { bc(i); return arr[i]; }
  T      & operator[] (int i)         { bc(i); return arr[i]; }

  // set size, reallocating if old size is different; if the
  // array gets bigger, existing elements are preserved; if the
  // array gets smaller, elements are truncated
  void setSize(int newSz);

  // make sure there are at least 'minSz' elements in the array;
  void ensureAtLeast(int minSz)
    { if (minSz > sz) { setSize(minSz); } }

  // grab a read-only pointer to the raw array
  T const *getArray() const { return arr; }

  // grab a writable pointer; use with care
  T *getDangerousWritableArray() { return arr; }
  T *getArrayNC() { return arr; }     // ok, not all that dangerous..

  // make sure the given index is valid; if this requires growing,
  // do so by doubling the size of the array (repeatedly, if
  // necessary)
  void ensureIndexDoubler(int index)
    { if (sz-1 < index) { eidLoop(index); } }

  // set an element, using the doubler if necessary
  void setIndexDoubler(int index, T const &value)
    { ensureIndexDoubler(index); arr[index] = value; }

  // swap my data with the data in another BitwiseGrowArray object
  void swapWith(BitwiseGrowArray<T> &obj) {
    T *tmp1 = obj.arr; obj.arr = this->arr; this->arr = tmp1;
    int tmp2 = obj.sz; obj.sz = this->sz; this->sz = tmp2;
  }

  // set all elements to a single value
  void setAll(T val) {
    for (int i=0; i<sz; i++) {
      arr[i] = val;
    }
  }

private:
  // call T constructor on [p, end)
  static void ctor(T* p, T *end) {
    for (; p < end; ++p) {
      ::new(static_cast<void*>(p)) T();
    }
  }

  // call T destructor on [p, end)
  static void dtor(T* p, T *end) {
    for (; p < end; ++p) {
      p->~T();
    }
  }

  // shallow copy data from src to dest
  static void copy(T *dest, T *src, size_t count) {
    memcpy(dest, src, sizeof(T)*count);
  }

  // uninitialized alloc
  static T *alloc(size_t count) {
    void *m = malloc(sizeof(T)*count);
    if (!m) throw bad_alloc();
    return (T*) m;
  }

  // dealloc
  static void dealloc(T *ary) {
    free(ary);
  }
};


template <class T>
BitwiseGrowArray<T>::BitwiseGrowArray(int initSz)
{
  sz = initSz;
  if (sz > 0) {
    // arr = new T[sz];
    arr = alloc(sz);
    ctor(arr, arr+sz);
  }
  else {
    arr = NULL;
  }
}


template <class T>
BitwiseGrowArray<T>::~BitwiseGrowArray()
{
  if (arr) {
    // delete[] arr;
    dtor(arr, arr+sz);
    dealloc(arr);
  }
}


template <class T>
void BitwiseGrowArray<T>::copyFrom_limit(BitwiseGrowArray<T> const &obj, int limit)
{
  for (int i=0; i<limit; i++) {
    arr[i] = obj.arr[i];
  }
}


template <class T>
void BitwiseGrowArray<T>::setSize(int newSz)
{
  if (newSz != sz) {
    // keep track of old
    int oldSz = sz;
    T *oldArr = arr;

    // make new
    sz = newSz;

    if (sz > 0) {
      // arr = new T[sz];
      arr = alloc(sz);
    }
    else {
      arr = NULL;
    }

    // // copy elements in common
    // for (int i=0; i<sz && i<oldSz; i++) {
    //   arr[i] = oldArr[i];
    // }

    if (sz < oldSz) {
      // shrinking; copy elements to keep and destroy the rest
      copy(arr, oldArr, sz);
      dtor(oldArr + sz, oldArr + oldSz);
    } else if (sz > oldSz) {
      // expanding; copy elements and construct new ones
      copy(arr, oldArr, oldSz);
      ctor(arr + oldSz, arr + sz);
    }

    // get rid of old
    if (oldArr) {
      // delete[] oldArr;
      dealloc(oldArr);
    }
  }
}


// this used to be ensureIndexDoubler's implementation, but
// I wanted the very first check to be inlined
template <class T>
void BitwiseGrowArray<T>::eidLoop(int index)
{
  if (sz-1 >= index) {
    return;
  }

  int newSz = sz;
  while (newSz-1 < index) {
    #ifndef NDEBUG_NO_ASSERTIONS    // silence warning..
      int prevSz = newSz;
    #endif
    if (newSz == 0) {
      newSz = 1;
    }
    newSz = newSz*2;
    xassert(newSz > prevSz);        // otherwise overflow -> infinite loop
  }

  setSize(newSz);
}


// ---------------------- BitwiseArrayStack ---------------------
// quarl 2006-06-03
// Analogous to ArrayStack, but uses a BitwiseGrowArray instead of GrowArray.
// TODO: make ArrayStack templatized on the base class
template <class T>
class BitwiseArrayStack : public BitwiseGrowArray<T> {
private:
  int len;               // # of elts in the stack

public:
  BitwiseArrayStack(int initArraySize = 10)
    : BitwiseGrowArray<T>(initArraySize),
      len(0)
    {}
  BitwiseArrayStack(BitwiseArrayStack<T> const &obj)
    : BitwiseGrowArray<T>(obj),
      len(obj.len)
    {}
  ~BitwiseArrayStack() {}

  // copies contents of 'obj', but the allocated size of 'this' will
  // only change when necessary
  BitwiseArrayStack& operator=(BitwiseArrayStack<T> const &obj)
  {
    this->ensureIndexDoubler(obj.length() - 1);
    this->copyFrom_limit(obj, obj.length());
    len = obj.len;
    return *this;
  }

  // element access; these declarations are necessary because
  // the uses of 'operator[]' below are not "dependent", hence
  // they can't use declarations inherited from GrowArray<T>
  T const& operator[] (int i) const { return BitwiseGrowArray<T>::operator[](i); }
  T      & operator[] (int i)       { return BitwiseGrowArray<T>::operator[](i); }

  void push(T const &val)
    { setIndexDoubler(len++, val); }
  T pop()
    { return operator[](--len); }
  T const &top() const
    { return operator[](len-1); }
  T &top()
    { return operator[](len-1); }
  T &nth(int which)
    { return operator[](len-1-which); }

  // alternate interface, where init/deinit is done explicitly
  // on returned references
  T &pushAlt()    // returns newly accessible item
    { BitwiseGrowArray<T>::ensureIndexDoubler(len++); return top(); }
  T &popAlt()     // returns item popped
    { return operator[](--len); }

  // items stored
  int length() const
    { return len; }

  bool isEmpty() const
    { return len==0; }
  bool isNotEmpty() const
    { return !isEmpty(); }

  void popMany(int ct)
    { len -= ct; xassert(len >= 0); }
  void empty()
    { len = 0; }

  // useful when someone has used 'getDangerousWritableArray' to
  // fill the array's internal storage
  void setLength(int L) { len = L; }

  // consolidate allocated space to match length
  void consolidate() { this->setSize(length()); }

  // swap
  void swapWith(BitwiseArrayStack<T> &obj) {
    BitwiseGrowArray<T>::swapWith(obj);
    int tmp = obj.len; obj.len = this->len; this->len = tmp;
  }

  void sort(int (*compare)(T const *t1, T const *t2)) {
    qsort(BitwiseGrowArray<T>::getArrayNC(), len, sizeof(T),
          (int (*)(void const*, void const*))compare );
  }
};



#endif // BITWISE_ARRAY_H
