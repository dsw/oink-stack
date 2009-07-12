// t0251.cc
// simple example with anonymous namespaces

// except the example was wrong... all anonymous namespaces within a
// translation unit (and in the same containing scope) are the same

namespace {
  int a;
  int b;
}

namespace N {
  int b;
  int c;
}
using namespace N;

void f()
{
  a;        // from first one
  //ERROR(1): b;        // ambiguous
  c;        // from second one
}
