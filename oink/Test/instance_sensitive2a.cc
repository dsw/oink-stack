struct A {
  int q;
};

int main() {
  struct A *a;
  struct A *a2;
  a = a2;                       // bad
  int $tainted x;
  int $untainted y;
  a->q = x;
  y = a2->q;
}
