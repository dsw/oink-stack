// return by pointer

struct A {};

A *f() {
  A $tainted *a = new A();
  return a;                     // bad
  return 0;                     // good
}

int main() {
  A $untainted *a2 = f();
}
