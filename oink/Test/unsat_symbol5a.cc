// test two-file unsatisfied symbol detection

struct A {
  void foo();
};

int main() {
  A a;
  a.foo();
}
