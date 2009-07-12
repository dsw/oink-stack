// member init ctors

struct A {
  A();
  A(int $untainted x);
};

struct B {
  A a;
  B(int y) : a(y) {}
};

int main() {
  int $tainted x;
  A a;                          // good
  A a(x);                       // bad
}
