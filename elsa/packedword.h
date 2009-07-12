// packedword.h            see license.txt for copyright and terms of use

// quarl 2006-06-22
//    PackedWord represents a single word (long for now; could be templatized)
//    as a bit vector.  This allows packing multiple objects into a single
//    word.

#ifndef WORDPACK_H
#define WORDPACK_H

#include "xassert.h"

class PackedWord {
public:
  typedef unsigned word;
  enum { WORD_SIZE = 8*sizeof(word) };

protected:
  // bit vector from [0, WORD_SIZE)
  word data;

  static word mask(int begin, int end) {
    return ((1<<(end-begin)) - 1) << begin;
  }

public:
  PackedWord() : data(0) {}

  // return data[begin, end)
  //
  // get(0, WORD_SIZE) == data
  word get(int begin, int end) const {
    word ret;
    if (end == WORD_SIZE) {
      // don't shift >> 32 cos it'd overflow
      ret = data >> begin;
    } else {
      ret = (data & mask(0,end)) >> begin;
    }
    return ret;
  }

  void set(int begin, int end, word data0) {
    xassert(begin < end && end <= WORD_SIZE);
    // make sure data0 isn't too big
    xassert((data0 & ~ mask(0,end-begin)) == 0);

    // mask out old value
    data = data & ~ mask(begin,end);

    // add in new value
    data = data | ( data0 << begin);
  }
};

#define PACKEDWORD_DEF_GS(PW, N, T, B, E)                         \
  T get##N() const {                                              \
    return (T) PW.get(B, E);                                      \
  }                                                               \
  void set##N(T t) {                                              \
    PW.set(B, E, (PackedWord::word) t);                           \
  }

#endif

// Local Variables:
// kc-compile-command-make: "make packedword_test && ./packedword_test"
// End:

