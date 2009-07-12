// pointer to data member contravariant rule
class A {
  public:
  int x;
  A(int x0) : x(x0) {}
};

class B : public A {
  public:
  int y;
  B(int x0, int y0) : A(x0), y(y0) {}
};

int main() {
  B b1(10, 11);
  int $tainted t;
  b1.x = t;

  int B::*q;
  q = &A::x;
  int $untainted z = b1.*q;     // bad
}
