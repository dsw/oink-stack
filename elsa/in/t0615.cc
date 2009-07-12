// t0615.cc
// related to t0614.cc; invalid code; triggers assertion failure

template <class T>
class A {
public:
  // problem only happens with an inline definition
  void foo() {}
};

// provoke an instantiation of A<int>::foo
void f()
{
  A<int> a;
  a.foo();
}

// now try to specialize it; boom!
template <>
void A<int>::foo()
{}

// EOF
