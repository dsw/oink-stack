// print hello world

#include "lib_barint.h"

#include <stdio.h>
#include <stdlib.h>

int main() {
  printf("main: Hello, world2!\n");
  int * *bar_holder = (int * *) malloc(sizeof *bar_holder);
  *bar_holder = new_Bar(3);
  int x = 0;
  x = weird_Bar(bar_holder);    // bad: should fail the trust test
  printf("x: %d\n", x);
  return 0;
}
