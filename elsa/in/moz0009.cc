// moz0009.cc
// sizeof applied to array sized by template param

// This is fine, but original Elsa gave an error:
// in/moz0009.cc:8:36: error: reprSize of a sizeless array 
//    (from template; would be suppressed in permissive mode)


template<int n>
struct A {
  char buf[n];

  int bufSize() { return sizeof(buf); }
};
