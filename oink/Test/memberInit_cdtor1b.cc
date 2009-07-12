// member init ctors

struct A {
  A(int $untainted x);
};

struct B : A {
  B(int y)
    : A(y)                      // bad
    : A(3)                      // good
  {}
};

int main() {
  int $tainted x;
  B b(x);
}
