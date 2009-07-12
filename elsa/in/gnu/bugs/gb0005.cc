// gb0005.cc
// failure to instantiate, failure to diagnose,
// though no diagnostic is required...

struct A {};

void f(A *x);

template <typename T>
struct B {
  void g(A const *p)
  {
    f(p);
  }
};


// this tests response to an error from overload resol'n
namespace N {
  int f(char);
  int f(short);

  template <class T>
  struct A {
    void g(int i)
    {
      f(i);
    }
  };
}
