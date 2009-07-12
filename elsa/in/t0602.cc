// t0602.cc
// cppstd ex 14.8.2.4p18.cc

template <template <class T> class X>
struct A { };

template <template <class T> class X>
void f(A<X>) { }

template<class T>
struct B { };

A<B> ab;

void foo() {
  f(ab);
}
