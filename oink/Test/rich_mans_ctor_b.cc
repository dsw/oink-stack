// this is poor_mans_ctor.c converted to OO; this should pass if we
// handle "this" specially

struct A {
  int q;
  A(int y) {
    q = y;
  }
};

int main() {
  int $tainted a;
  int a2;
  A x1(a);
  A x2(a2);

  int $untainted z =
    x1                          // bad
    x2                          // good
    .q;
}
