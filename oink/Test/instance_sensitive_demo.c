struct A {
  int q;
};

int main() {
  struct A *a1;
  struct A *a2;
  int $tainted x;
  int $untainted y;
  a1->q = x;
  // NOTE that we do not do this: a2 = a1;
  y = a2->q;
}
