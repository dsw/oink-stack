// see License.txt for copyright and terms of use

#ifndef CXXCOROUTINE_H
#define CXXCOROUTINE_H

// quarl 2006-04-28
//    Macros for coroutines in C++.
//
//    Based on http://www.chiark.greenend.org.uk/~sgtatham/coroutines.html

#include <assert.h>

class Coroutine {
protected:
  Coroutine() : crLine(0) { init(); }
  int crLine;
#ifndef NDEBUG
  // avoid recursion within the same Coroutine
  bool entered;
  void enter() { assert(!entered); entered = true; }
  void leave() { assert(entered); entered = false; }
  void init() { entered = false; }
#else
  void enter() {}
  void leave() {}
  void init() {}
#endif
};

#define cxxCrBegin         enter(); switch(crLine) { case 0:;
#define cxxCrFinish(z)     default: assert(0); } leave(); return (z)
#define cxxCrFinishV       default: assert(0); } leave(); return

#define cxxCrReturn(z)                                            \
  do {                                                            \
    crLine=__LINE__;                                              \
    leave();                                                      \
    return (z); case __LINE__:;                                   \
  } while (0)

#define cxxCrReturnV                                              \
  do {                                                            \
    crLine=__LINE__;                                              \
    leave();                                                      \
    return; case __LINE__:;                                       \
  } while (0)

#endif
