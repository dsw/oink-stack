// compoundinit1A.c; see License.txt for copyright and terms of use

// test compound initializers
int main() {
  // non-designated
  int *ip;// = 0;         // NULL
  struct foo {int x; int *xp; int y;};
  struct foo f = {1, ip, 2};
}
