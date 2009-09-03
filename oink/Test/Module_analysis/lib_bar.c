#include "lib_bar.h"

#include <stdio.h>
#include <stdlib.h>

// ctor
struct Bar *new_Bar(int q0) {
  struct Bar *ret = (struct Bar *) malloc(sizeof *ret);
  ret->q = q0;
  return ret;
}

// method
int get_q_Bar(struct Bar *b) {
  return b->q;
}

// this does something rather weird with b that should fail the trust
// analysis
int weird_Bar(struct Bar **b) {
  return (*b)->q;
}
