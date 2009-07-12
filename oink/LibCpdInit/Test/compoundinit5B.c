// compoundinit5B.c; see License.txt for copyright and terms of use

void funb() {
  struct foo2 {
    struct bar2 {
      char a2[10];
    } b2;
  };
  struct foo2 f2 = {
    'h', 'e', 'l', 'l', 'o'
  };
}
