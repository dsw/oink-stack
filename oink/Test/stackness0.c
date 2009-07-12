// control test: can we pass the address of a stack pointer somewhere
// without causing a spurious warning?

// #include <stdlib.h>

struct A {
  int *x;
};

void f(int *q) {
  struct A *h = (struct A*) malloc(sizeof(h[0]));
  // omit this
//   h->x = q;
}

int main() {
  int s;
  f(&s);
  return 0;
}
