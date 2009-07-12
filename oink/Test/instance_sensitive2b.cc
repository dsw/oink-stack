struct A {
  int q;
};

int main() {
  struct A *a;
  struct A *a2;
  int $tainted x;
  int $untainted y;
  a->q = x;
  a = a2;                       // bad
  y = a2->q;
}
