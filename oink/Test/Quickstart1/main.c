#include "g.h"
#include <stdio.h>
#include <stdlib.h>

void f(int x, char *data) {
  if (x) {
    g(data);
  }
}

int main() {
  printf("Hello, World!");
  char *data = getenv("HELLO");
  f(1, data);
  return 0;
}
