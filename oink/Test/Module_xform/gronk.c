// the point of this test is to just use malloc

#include "gronk.h"
#include "gronk_b.h"

#include <stdio.h>
#include <stdlib.h>

void f(struct Gronk *g) {
  printf("This is a Gronk::f().  The value of g->x is %d.\n", g->x);
}

int main() {
  // Gronk
  printf("ctor Gronk.\n");
  struct Gronk *g = (struct Gronk *) malloc(sizeof *g);
  g->x = 3;

  printf("Gronk::f.\n");
  f(g);

  printf("dtor Gronk.\n");
  free(g);

  // Gronk_B
  printf("\n");
  printf("ctor Gronk_B.\n");
  struct Gronk_B *gb = new_Gronk_B(5, 7);

  printf("gb->y %d\n", get_y(gb));

  set_y(gb, 6);
  printf("gb->y %d\n", get_y(gb));

  printf("dtor Gronk_B.\n");

  return 0;
}
