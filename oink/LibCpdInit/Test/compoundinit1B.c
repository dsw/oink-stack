// compoundinit1B.c; see License.txt for copyright and terms of use

// test compound initializers
int main() {
  // non-designated
  int *ip;// = 0;         // NULL
  // designated
  struct foo2 {int x2; int *xp2; int y2;};
  struct foo2 f2 = {y2:2, x2:1, xp2:ip};
}
