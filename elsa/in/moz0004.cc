// moz0004.cc
// make sure Elsa doesn't give a spurious template typecheck error
// when a short-circuit operator is used in a template.

template<int K>
class A {
  int a[K >= 2 && K <= 15 ? 1 : -1];
};

A<3> a;
