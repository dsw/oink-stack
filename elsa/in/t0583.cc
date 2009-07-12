// t0583.cc
// compare partial specializations using non-type arguments

// based on a report by Gustav Munkby

template <int i, int j>
struct A {};

template <int i>
struct A<i, 0> {};

template <>
struct A<0, 0> {
  int x;
};

int foo()
{
  A<0,0> a;
  return a.x;
}

