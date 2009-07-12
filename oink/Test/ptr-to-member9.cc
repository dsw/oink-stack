// pointer to member operator associativity
class A {
  public:
  int x;
  A(int x0) : x(x0) {}
};

// NOTE: does NOT inherit from A
class C {
  public:
  int A::*q;
  C(int A::*q0) : q(q0) {}
};

int main() {
  A &a0 = *(new A(17));
  C &c0 = *(new C(&A::x));
  int A::* C::* r;
  r = &C::q;
//    return a0 .* c0 .* r;         // should not typecheck
  return a0 .* (c0 .* r);       // should typecheck
}
