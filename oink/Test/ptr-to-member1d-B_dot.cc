// pointer to data member
class A {
  public:
  int x;
  A(int x0) : x(x0) {}
};

int main() {
  A a0(1);
  int $tainted t;
  a0.x = t;

  int A::*q;
  q = &A::x;
  int $untainted y = a0.*q;     // bad
}
