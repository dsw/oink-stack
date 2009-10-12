// moz0007.cc
// The gcc extended offsetof operator.

struct B {
  int x;
};

struct A {
  int a;
  B b;
};

// b.x allowed only with gcc extension
int x = __builtin_offsetof(A, b.x);
int q = __builtin_offsetof(A, b);
