// t0580.cc
// use of an enumerator in a template-id naming a specialization

enum E { one=1 };

template <int n>
struct A {};

template <>
struct A<1> {
  typedef int INT;
};

A<one>::INT i;

