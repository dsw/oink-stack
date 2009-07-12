// test file for pretty printing (can't just print fully-qualified name of the
// second E1)

namespace {
  enum E1 { };

  struct S1 {
    enum E1 { };

    S1(E1) {}
  };
}
