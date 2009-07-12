struct A {
  int r;
};

struct B {
  int r;
};

int main() {
  struct A                      // bad
  struct B                      // good
    * x;

  int $tainted t;
  int $untainted u;

  x->r = t;

  void * q;
  q = x;
  struct A *y = (struct A*) q;

  u = y->r;
}
