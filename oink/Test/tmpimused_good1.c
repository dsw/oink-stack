int $!useimmediately *z;        // global

struct A {
  int $!useimmediately *q;
};

void f(int $!useimmediately *);

void f2(int $!useimmediately *x) {
  *x;
  int $!useimmediately *y = x;
}

void g() {
  struct A a;
  struct A *a2;
  int $!useimmediately *x2;
  *x2;
  *z;
  *(a.q);
  *(a2->q);
  int $!useimmediately *y = x2;
  int $!useimmediately *y2 = z;
}
