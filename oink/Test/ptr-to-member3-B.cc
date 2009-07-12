// pointer to function member contravariant rule
class A {
  public:
  int f(int x) {                // NOT virtual
    int $untainted y = x;
  }
};

class B : public A {
  public:
  int f(int x) {
  }
};

int main() {
  B *b1 = new B();
  int (B::*q)(int);
  // NOTE: A, not B
  q = &A::f;
  int $tainted x = 0;
  (b1->*q)(x);                  // bad
}
