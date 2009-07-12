// do we find a pointer to the stack stored in the heap?

#include <stdlib.h>

int main() {
  int s;
  int **h = (int**) malloc(sizeof(h[0]));
  *h = &s;
  return 0;
}
