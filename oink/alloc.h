// see License.txt for copyright and terms of use

#ifndef ALLOC_H
#define ALLOC_H

// quarl 2006-06-14
//    IterableBlockAllocator is a pool allocator for small heterogeneous
//    objects.
//
//    Struct<T1,T2> can be used to make sure two objects of type T1,T2 are
//    allocated back-to-back (i.e. are not split across a Block allocation
//    boundary).
//
//    It supports fast linear traversal.  Traversal occurs in the same order
//    as allocation.
//
//    It does not support deallocation, for now, since we allow arbitrary
//    heterogeneous objects, so we can't destruct.  We could support this by
//    requiring a base class with a virtual destructor or some other
//    user-supported method.

#include "exc.h"
#include <stddef.h>             // offsetof

// quarl 2006-06-16: Product Struct
//    Struct is a data structure that represents the product of two (or more)
//    objects.  They are allocated consecutively, but none need to be
//    "embedded" in others, or even know about each other except at allocation
//    time.  This is similar to non-virtual multiple inheritance.
//
//    &t1 can be derived from &t2 and vice versa.  Can be generalized to more
//    members.  Any member can be "upcasted" to the parent Struct, even if
//    there is more than one member of the same type -- something you can't do
//    with multiple inheritance.
//
//    This is an alternative to two objects, allocated separately, that point
//    to each other.  This saves the memory used by the pointers, time to
//    dereference those pointers, and also reduces memory fragmentation and
//    improves cache performance.
//
template <class T1_, class T2_>
struct Struct {
  typedef T1_ T1;
  typedef T2_ T2;
  typedef Struct<T1_,T2_> MyStruct;
  T1 t1;
  T2 t2;

  // Apparently having a constructor makes Struct non-POD.  However, the
  // warning that gcc gives (conforming to spec) about using offsetof in a
  // non-POD type is over-conservative.  Offsetof is still well-defined in the
  // case of "POD type with constructor", and in this case where Struct is a
  // POD struct of potentially non-PODs.
  //
  // http://gcc.gnu.org/ml/gcc/2003-04/msg00970.html

  // An alternative would be to assume that the objects are back-to-back and
  // just use pointer arithmetic (&t2-1); or to guarantee they are
  // back-to-back by manually creating a struct ("char[sizeof(T1)+sizeof(T2)]")

// #define Struct_offsetof(Type,member) (&((Type*)0)->member)

  // Return pointer to parent struct given a member.  All types above the
  // argument must match, i.e. you can do Struct<T1,
  // anything>::getStructFrom2(x)

  static inline MyStruct *getStructFromPtr(void *t, int offset)
  {
    return reinterpret_cast<MyStruct*>((reinterpret_cast<char*>(t) - offset));
  }

  static MyStruct *getStructFrom1(T1 *t1)
  {
    // int offset = offsetof(MyStruct, t1);
    // xassert(offset == 0);
    int offset = 0;
    MyStruct *s = getStructFromPtr(t1, offset);
    xassert(&s->t1 == t1);
    return s;
  }

  static MyStruct *getStructFrom2(T2 *t2)
  {
    // int offset = offsetof(MyStruct, t2);
    // xassert(offset == sizeof(T1));
    int offset = sizeof(T1);
    MyStruct *s = getStructFromPtr(t2, offset);
    xassert(&s->t2 == t2);
    return s;
  }
} /*__attribute__((packed)) */;

class IterableBlockAllocator
{
  // some multiple of 2, minus the size of a pointer, so that the total size
  // of Block is a multiple of 2.
  enum { BLOCK_SIZE = 512*1024-sizeof(void*) };
  typedef char byte;

  // A block of objects, with a 'next' pointer
  struct Block {
    byte data[BLOCK_SIZE];
    Block *nextBlock;
    Block() : nextBlock(NULL) {
      memset(&data, 0, sizeof(data));
      // fprintf(stderr, "## new Block: begin=%p, end=%p\n", begin(), end());
    }

    byte *begin() { return &data[0]; }
    byte *end() { return &data[BLOCK_SIZE]; }
  };

  struct AllocHeader {
    size_t size;
  };

  // Head of block linked list
  Block *headBlock;

  // Tail of block linked list
  Block *tailBlock;

  // Pointer to within headBlock->data
  byte *allocNext;

  // number of times alloc() was called
  size_t allocCount;

  // Create a new block.  Must not be the first block created.
  void newBlock() {
    xassert(headBlock && tailBlock);
    Block *oldTail = tailBlock;
    oldTail->nextBlock = tailBlock = new Block();
    allocNext = tailBlock->begin();
  }

  void newBlockFirst() {
    headBlock = tailBlock = new Block();
    allocNext = tailBlock->begin();
  }

  // allocate N bytes, uninitialized
  void *allocBytes(size_t N) {
    xassert(N > 0);
    xassert(N < BLOCK_SIZE);
    xassert(allocNext <= tailBlock->end());
    // TODO: pad N to wordsize
    if (allocNext + N + sizeof(AllocHeader) > tailBlock->end()) {
      if (allocNext != tailBlock->end()) {
        // Should already be 0 since we memset(), but don't depend on that
        // memset always being there.  The 0 is important because we look for
        // it when we iterate.
        ((AllocHeader*)(allocNext))->size = 0;
      }
      newBlock();
    }
    // quarl 2006-06-20
    //    Store the size of the object to be allocated.  This wastes 4 bytes,
    //    but we need it later when we iterate.
    // quarl 2006-06-21
    //    We could get rid of this by 1) combining the type tag with it, or 2)
    //    require the user to specify a "max size" of objects so that when
    //    iterating we can jump to the next block if maxSize bytes wouldn't
    //    fit.  For big block sizes this would waste at most (maxSize-minSize)
    //    per block.
    ((AllocHeader*)(allocNext))->size = N;
    void *ret = allocNext + sizeof(AllocHeader);
    allocNext += N + sizeof(AllocHeader);
    // fprintf(stderr, "## allocBytes(%d) = %p; allocNext=%p\n", N, ret, allocNext);
    return ret;
  }

public:
  IterableBlockAllocator() : headBlock(0), allocCount(0) { newBlockFirst(); }

  // number of objects allocated so far
  size_t getAllocCount() const {
    return allocCount;
  }

  // allocate and construct one object of type T
  template <class T>
  T *alloc() {
    T *ret = static_cast<T*>(allocBytes(sizeof(T)));
    ::new(static_cast<void*>(ret)) T(); // call constructor
    ++allocCount;
    return ret;
  }

  class Iter {
    Block *block;
    byte *next;
    byte *last;

    // How many items are left to iterate through.
    //
    // quarl 2006-06-20
    //    This is useful because the size given to adv() depends on virtual
    //    functions and such which is a common source of bugs.
    size_t remaining;

  public:
    Iter(IterableBlockAllocator &allocator)
      : block(allocator.headBlock),
        next(block->begin()),
        last(allocator.allocNext),
        remaining(allocator.getAllocCount())
    {}

    bool isDone() const {
      xassert((remaining==0) == (next==last));
      if (remaining == 0) xassert(block->nextBlock==NULL);
      return (remaining == 0);
    }

    size_t dataSize() const { return ((AllocHeader*)next)->size; }

    void *data0() const {
      xassert(!isDone());
      xassert(next + sizeof(AllocHeader) + dataSize() <= block->end());
      return next + sizeof(AllocHeader);
    }

    template <class T>
    T *data() const { return (T*) data0(); }

    // advance the read pointer.
    void adv() {
      xassert(!isDone());
      selfCheck();
      remaining--;
      size_t N = dataSize();
      xassert(N > 0 && N < BLOCK_SIZE);
      byte *newNext = next + N + sizeof(AllocHeader);
      // Make sure we didn't pass the end of allocation.
      if (block->begin() <= last && last <= block->end()) {
        xassert(newNext <= last);
      }

      // The previous object should have been within the block
      xassert(newNext <= block->end());

      // Would the next object put us outside the current block?  If so, we
      // would have stored a "0" marker as the size.
      if (newNext == block->end() || ((AllocHeader*)newNext)->size == 0) {
        // We should never reach a NULL next pointer, since we're not done.
        if (remaining == 0) {
          xassert(newNext == last);
          xassert(block->nextBlock == NULL);
        } else {
          xassert(block->nextBlock != NULL);
          block = block->nextBlock;
          newNext = block->begin();
        }
      }
      // fprintf(stderr, "## adv: %p + %d = %p\n", next, N, newNext);
      next = newNext;
      selfCheck();
    }

    void selfCheck() {
      if (remaining == 0) {
        xassert(next == last);
        xassert(block->nextBlock == NULL);
      } else {
        xassert(next != last);
        xassert(next >= block->begin() && next < block->end());
      }
    }
  };
};


#endif
