struct A {
  int r;
};

struct B {
  int r;
};

int f(int p) {
  return p;
}

int main() {
  struct A                      // bad
  struct B                      // good
    * x;

  int $tainted t;
  int $untainted u;

  x->r = t;

  struct A *y = (struct A*) f( (int) x);

  u = y->r;
}
