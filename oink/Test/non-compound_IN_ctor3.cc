// test that a Declarator correctly initializes a non-compound type,
// here a function pointer, within an IN_ctor; also tests that the
// call through the function pointer calls it as a function pointer
// and not as a function

int f(int x) {
  return x;
}

int main() {
  int (*f1)(int)(f);            // an IN_ctor for a non-compound type
  int $tainted t = 0;
  int $untainted u1;
  u1 = f1(t);                   // call through function pointer; NOT a function call!
  return 0;
}
