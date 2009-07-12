// gb0009.cc
// failure to implement the defn scope must enclose decl scope rule
// (7.3.1.2p2 and 9.3p2)

// variant with named namespaces; ICC rejects this
namespace N
{
  struct A {
    void f(int);
  };
}
using namespace N;

namespace M
{
  void A::f(int) {}
}


// variant with anonymous namespaces; even ICC does not reject
namespace
{
  struct B {
    void f(int);
  };
}

namespace
{
  void B::f(int) {}
}


