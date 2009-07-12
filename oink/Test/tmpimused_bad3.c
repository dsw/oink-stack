struct A {
  int $!useimmediately *x;
};

void f() {
  struct A *a;
  a->x;
}
