// print hello world

#include "lib_bar.h"

#include <stdio.h>
#include <stdlib.h>

struct Bar;                     // forward

int main() {
  printf("main: Hello, world2!\n");
  struct Bar * *bar_holder = (struct Bar * *) malloc(sizeof *bar_holder);
  *bar_holder = new_Bar(3);
  int x = weird_Bar(bar_holder); // should fail the trust test
  printf("x: %d\n", x);
  return 0;
}
