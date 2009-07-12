// test the presence of an E_compoundLit in an compound initializer

// minimized from the kernel
struct A {
  int a0;
  int a1;
};

struct C {
  struct A x;
  struct A y;
};

struct C c[] =
{
  (struct A) {0, 1},
  {2, 3},
};

struct C c2[] =
{
  ((struct A) {4, 5}),
  {6, 7},
};
