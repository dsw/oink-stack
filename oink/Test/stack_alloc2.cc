// test if we can tell what variables are allocated on the stack

int x;                          // global: no

struct Foo {
  int q;                        // class: no, as depends on container
  static int r;                 // static class == global: no
};

void f(int x,
       int x2)                  // paramter: stack
{
  int *x2_p = &x2;              // and addr taken: yes
  int y;                        // function/auto: stack
  int y2;
  int *y2_p = &y2;              // and addr taken: yes
  static int z;                 // function/static == global: no
  ++x;                          // use
  Foo foo1;                     // function/auto: stack
  Foo *foo1_p = &foo1;          // and addr taken: yes
  Foo foo2;                     // function/auto: stack, no addr taken
  Foo foo3;                     // function/auto: stack
  int *foo3_q_p = &(foo3.q);    // and addr taken: yes
  Foo foo4;                     // function/auto: stack
  int *foo4_r_p = &(foo4.r);    // and static addr taken: no
}

template<class T>               // template param: no
class Quux {
  T w;                          // class: no
};

Quux<int> Q1;                   // global: no

template<class T>               // template param: no
T gronk(T a, T a2, int a3) {    // param: stack
  T *a2_p = &a2;                // and addr taken: yes
  int *a3_p = &a3;              // and addr taken: yes
  return a;                     // use
}

void blort() {
  gronk(3, 4, 5);               // instantiate gronk
}

namespace Bar {
  int v;                        // namespace: no
};
