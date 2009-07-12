// compoundinit1H.c; see License.txt for copyright and terms of use

// test compound initializers
int main() {
  // non-designated
  int *ip;// = 0;         // NULL
  // array with no size
  struct foo10d {int xd; int yd[3];};
  struct foo10d f10d = {3, 4, 5, 6};
  struct foo10e {int xe; int ye[3]; int ze; int z2e;};
  struct foo10e f10e = {7, {8, 9}, 12, 11};
}
