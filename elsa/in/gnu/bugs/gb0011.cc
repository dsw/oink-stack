// gb0011.cc
// gcc accepts duplicate parameter names in prototypes

int f(int x, int *x);

// it rejects this, at least, though now Elsa does not
// when it is emulating the GCC bug
//int f(int x, int *x) { return 1; }

void foo(int *p)
{
  f(2,p);
}

struct A {
  int f(int x, int x);
};

//ERROR(1): int A::f(int x) { return 1; }

int A::f(int x, int xx)
{
  return 2;
}
