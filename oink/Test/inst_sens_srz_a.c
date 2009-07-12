// check that int-as-void* works across serialization

struct A {
  int x;
};

void f(struct A *a) {
  int $untainted b = a->x;
}
