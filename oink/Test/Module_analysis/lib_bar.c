#include "lib_bar.h"

#include <stdio.h>
#include <stdlib.h>

// FIX: this should be in lib_bar.h but we are not using the
// preprocessor right now; NOTE: this will analyze, but it won't build

// class
struct Bar {
  int q;
};

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
