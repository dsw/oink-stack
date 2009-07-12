// test that a Declarator correctly initializes a non-compound type,
// here a function pointer, within an IN_ctor; also tests that the
// call through the function pointer calls it as a function pointer
// and not as a method, which is what it looks like

int f(int x) {
  return x;
}

struct A {
  int (*f1)(int);
  A()
    : f1(f)                     // an IN_ctor for a non-compound type
  {}
};

int main() {
  A a;
  int $tainted t = 0;
  int $untainted u1;
  u1 = a.f1(t);                 // call through function pointer; NOT a method call!
  return 0;
}
