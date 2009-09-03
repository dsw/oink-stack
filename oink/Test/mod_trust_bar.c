#include "lib_barint.h"

#include <stdio.h>
#include <stdlib.h>

// ctor
int *new_Bar(int q0) {
  int *ret = (int *) malloc(sizeof *ret);
  *ret = q0;
  return ret;
}

// method
int get_q_Bar(int *b) {
  return *b;
}

// this does something rather weird with b that should fail the trust
// analysis
int weird_Bar(int **b) {
  return **b;
}
