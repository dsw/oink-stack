// test if we can tell what variables are allocated on the stack

int x;                          // global: no

struct Foo {
  int q;                        // class: no, as depends on container
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
}
