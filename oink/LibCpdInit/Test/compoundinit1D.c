// compoundinit1D.c; see License.txt for copyright and terms of use

// test compound initializers
int main() {
  // non-designated
  int *ip;// = 0;         // NULL
  // non-designated compound literal
  struct foo2B {int x2B; int *xp2B; int y2B;};
  struct foo2B f2B;
  f2B = (struct foo2B) {y2B:2, x2B:1, xp2B:ip};
}
