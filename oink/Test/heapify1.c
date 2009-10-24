// test if we can tell what variables are allocated on the stack

int x;                          // global: no

struct Foo {
  int q;                        // class: no, as depends on container
};

void f(int x,
       int x2)                  // parameter: stack
{
  int *x2_p = &x2;              // and addr taken: yes
  int y;                        // function/auto: stack
  int y2;
  int *y2_p = &y2;              // and addr taken: yes
  static int z;                 // function/static == global: no
  ++x;                          // use
  struct Foo foo1;              // function/auto: stack
  struct Foo *foo1_p = &foo1;   // and addr taken: yes
  struct Foo foo2;              // function/auto: stack, no addr taken
  struct Foo foo3;              // function/auto: stack
  int *foo3_q_p = &(foo3.q);    // and addr taken: yes
}
