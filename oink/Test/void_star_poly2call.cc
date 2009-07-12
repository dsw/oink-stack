struct A {
  int r;
};

struct B {
  int r;
};

void *f(void *p) {
  return p;
}

int main() {
  struct A                      // bad
  struct B                      // good
    * x;

  int $tainted t;
  int $untainted u;

  x->r = t;

  struct A *y = (struct A*) f(x);

  u = y->r;
}
