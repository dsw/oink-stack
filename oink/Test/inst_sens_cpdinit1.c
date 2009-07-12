struct A {
  int q;
};

int main() {
  int $tainted x;
  struct A a = { .q = x };
  struct A *a2;
  a2 = &a;                      // bad
  int $untainted y;
  y = a2->q;
}
