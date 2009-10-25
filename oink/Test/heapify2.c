#include <stdlib.h>

struct Foo {
  int q;                        // class: no, as depends on container
};

int main() {
//   int *x = 3;
//   &x;

//   struct Foo foo4[3];           // function/auto: stack
//   &(foo4[2].q);                 // and addr taken: yes
  struct Foo (*foo4)[3]=malloc(sizeof *foo4);
  int *q_p = &((*foo4)[2].q);
  return *q_p;
}
