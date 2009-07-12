// check that int-as-void* works across serialization

struct A {
  int x;
};

void f(struct A *a);

int main() {
  struct A a;
  int $tainted t;
  a.x = t;
  f(&a);
}
