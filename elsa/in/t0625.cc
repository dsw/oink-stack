// t0625.cc
// anonymous namespaces all refer to the same namespace!

namespace {
  struct A;
  A *a1;
  void f1(A*) {}
}

// This is the same namespace as the anonymous namespace preceding it!
namespace {
  struct A;
  A *a2;
  void f2(A*) {}
}

void f()
{
  A *a;

  a1 = a2;
  a2 = a1;

  f1(a2);
  f2(a1);

  f1(a);
  f2(a);
}

// EOF
