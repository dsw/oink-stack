// compoundinit1G.c; see License.txt for copyright and terms of use

// test compound initializers
int main() {
  // non-designated
  int *ip;// = 0;         // NULL
  // nested int array and struct; top level designated
  struct foo3c {int x3c; int y3c;};
  struct foo4c {int x4c; int y4c;};
  struct gronkc {
    struct foo3c f3c;
    int zc[3];
    struct foo4c f4c;
  };
  struct gronkc gc = {
    zc:{3, 4, 5,},
    f4c:{6, 7,},
    f3c:{y3c:2, x3c:1},
  };
}
