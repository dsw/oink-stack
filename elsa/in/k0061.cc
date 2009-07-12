// multiple base classes instantiated from the same template class

// originally found in package kdelibs

// k0061.cc:19:12: error: reference to `S' is ambiguous, because it could either refer to S::S or S::S
// typechecking results:
//   errors:   1
//   warnings: 0

// ERR-MATCH: error: reference to `.*' is ambiguous, because it could either refer to

template <class T>
struct S {
  void bar() {}
};

struct C : S<int>, S<float> {
  void foo() {
    S<float>::bar();
  }
};
