// compoundinit1F.c; see License.txt for copyright and terms of use

// test compound initializers
int main() {
  // non-designated
  int *ip;// = 0;         // NULL
  // embedded and nested int array and struct
  struct foo3b {int x3b; int y3b;};
  struct foo4b {int x4b; int y4b;};
  struct gronkb {
    struct foo3b f3b;
    int zb[3];
    struct foo4b f4b;
  };
  struct gronkb gb = {
    1, 2,        // f3b
    {3, 4, 5,},  // zb
    {6, 7,}      // f4b
  };
}
