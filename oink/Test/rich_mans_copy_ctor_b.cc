// this should pass if we handle "this" specially; testing default
// copy ctors

struct A {
  int q;
  A(A const &other)
    : q(other.q)                // bad
  {}
};

int main() {
  int $tainted t;
  int $untainted u;

  A *a1;
  A a2(*a1);
  A a3(a2);
  A a4(a3);

  a1->q = t;
  u = a4.q;
}
