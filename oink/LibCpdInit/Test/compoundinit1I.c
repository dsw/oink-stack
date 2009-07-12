// compoundinit1I.c; see License.txt for copyright and terms of use

// test compound initializers
int main() {
  // non-designated
  struct gronkb {
    int zb[3];
  };
  struct gronkb gb = {
    {1,},  // zb
  };
}
