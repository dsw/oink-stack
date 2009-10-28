// test if we can tell what variables are allocated on the stack

int x;                          // global: no

struct Foo {
  int q;                        // class: no, as depends on container
};

struct Bar {
  int q2[10];
};

struct Gronk {
  struct Foo f;
};

void f(int x1,
       int x2)                  // parameter: stack
{
  ++x1;                         // use

  &x2;                          // and addr taken: yes

  int y;                        // function/auto: stack

  int y2;                       // function/auto: stack
  &y2;                          // and addr taken: yes

  int y3 = 3;                   // function/auto: stack
  &y3;                          // and addr taken: yes

  int *y4[3];                   // function/auto: stack
  &y4;                          // and addr taken: yes

  int *y5 = &x;                 // function/auto: stack
  &y5;                          // and addr taken: yes

  static int z;                 // function/static == global: no

  struct Foo foo1;              // function/auto: stack
  &foo1;                        // and addr taken: yes

  struct Foo foo2;              // function/auto: stack, no addr taken

  struct Foo foo3;              // function/auto: stack
  &(foo3.q);                    // and addr taken: yes

  int q1, q2, q3;               // function/auto: multiple stack
  &q2;                          // and addr taken: yes

  int qx1, qx2=2, qx3;          // function/auto: multiple stack
  &qx2;                         // and addr taken: yes

  int s1, s2, s3, s4=4, s5, s6=6, s7; // function/auto: multiple stack
  &s2;                                // and addr taken: yes
  &s4;
//   &s6;

  int r[2][3][4][5];            // function/auto: stack
  // note: this is deliberately partial
  &r[1][2][3];                  // and addr taken: yes

  struct Foo foo4[3];           // function/auto: stack
  &(foo4[2].q);                 // and addr taken: yes

  struct Bar bar1;              // function/auto: stack
  &(bar1.q2[3]);                // and addr taken: yes

  struct Bar bar2;              // function/auto: stack
  bar2.q2[3];                   // and addr taken: NO

  struct Gronk gronk1;          // function/auto: stack
  &(gronk1.f.q);                // and addr taken: yes

  struct Gronk gronk2;          // function/auto: stack
  gronk1.f.q;                   // and addr taken: NO

  struct Foo foo5 = { .q = 3 }; // function/auto: stack, but compound init
  &foo5.q;                      // and addr taken: yes
}

void g() {
  int x[10];                    // no addr taken
  x[3];
  *(x+4);

  int y[10];                    // no addr taken
  sizeof(y);
  sizeof(&y);
  sizeof(&(y[3]));

  int z[10];                    // addr taken
  &z[3];

  int z2[10];                   // addr taken
  &(*(z2+3));

  int z3[10];
  z3;                           // addr taken
}
