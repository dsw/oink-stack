// test assigning to arrays of characters
void funa() {
  struct foo {
    struct bar {
      char a[];                   // array of tainted chars
    } b;
  };
  struct foo f = {
    (char $tainted []) "hello"
  };
}

void funb() {
  struct foo2 {
    struct bar2 {
      char a2[];                  // array of tainted chars
    } b2;
  };
  struct foo2 f2 = {
    'h', (char $tainted) 'e', 'l', 'l', 'o'
  };
}
