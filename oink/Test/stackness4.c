// do we find a pointer to the stack stored in the heap when
// indirecting through a function call?

// #include <stdlib.h>

struct A {
  int *x;
};

void f(int *q) {
  struct A *h = (struct A*) malloc(sizeof(h[0]));
  h->x = q;
}

int main() {
  int s;
  f(&s);
  return 0;
}
