// do we find a pointer to the stack stored in the heap in a struct?

// #include <stdlib.h>

struct A {
  int *x;
};

int main() {
  int s;
  struct A *h = (struct A*) malloc(sizeof(h[0]));
  h->x = &s;
  return 0;
}
