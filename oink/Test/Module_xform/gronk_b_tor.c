// constructor/destructor for Gronk_B

#include "gronk_b.h"

#include <stdlib.h>

struct Gronk_B *new_Gronk_B(int y0, int z0) {
  // allocate
  struct Gronk_B *g = (struct Gronk_B*) malloc(sizeof (struct Gronk_B));
  // initialize
  g->y = y0;
  g->z = z0;
  // return
  return g;
}

void del_Gronk_B(struct Gronk_B *g) {
  free(g);
}
