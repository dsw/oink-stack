// t0620.cc
// explicitly instantiate a template member

class A {
public:
  template <class T>
  void foo(T v) {}

  template <class T>
  void foo2(T v) {}

  template <class T>
  void bar(T v);

  template <class T>
  void bar2(T v);
};

// template args determined by argument deduction
template void A::foo(int);

// explicit template args
template void A::foo2<int>(int);


// try one with an out-of-line definition too
template <class T>
void A::bar(T v)
{}

template void A::bar(int);


template <class T>
void A::bar2(T v)
{}

// and out-of-line with explicit template args
template void A::bar2<int>(int);


// EOF
