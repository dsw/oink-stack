// compoundinit1C.c; see License.txt for copyright and terms of use

// test compound initializers
int main() {
  // non-designated
  int *ip;// = 0;         // NULL
  // non-designated compound literal
  struct fooB {int xB; int *xpB; int yB;};
  struct fooB fB;
  fB = (struct fooB) {1, ip, 2};
}
