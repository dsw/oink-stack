#include "g.h"
#include <stdio.h>

void g(char *data) {
  printf("  The important data is ");
  printf(data);                 // DANGEROUS!
  printf("\n");
}
