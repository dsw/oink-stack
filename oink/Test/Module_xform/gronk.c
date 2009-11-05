// the point of this test is to just use malloc

#include "gronk.h"

#include <stdio.h>
#include <stdlib.h>

void f(struct Gronk *g) {
  printf("This is a gronk f().  The value of g->x is %d.\n", g->x);
}

int main() {
  printf("Gronk main.\n");
  struct Gronk *g = (struct Gronk *) malloc(sizeof *g);
  g->x = 3;
  f(g);
  free(g);
  printf("Gronk main done.\n");
  return 0;
}
