// stack ctors

struct A {
  A(int $untainted a);
};

int main() {
  int $tainted x;
  A a(
      x                         // bad
      3                         // good
      );
}
