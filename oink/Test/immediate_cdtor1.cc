// immediate temporary ctors

struct A {
  A(int $untainted a);
};

int main() {
  int $tainted x;
  A a = A(
          x                     // bad
          3                     // good
          );
}
