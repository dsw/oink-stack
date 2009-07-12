// test compound initializers
int main() {
  // non-designated
  int $tainted *ip = 0;         // NULL
  struct foo {int x; int *xp; int y;};
  struct foo f = {1, ip, (int $tainted) 2};

  // designated
  struct foo2 {int x2; int *xp2; int y2;};
  struct foo2 f2 = {y2:(int $tainted) 2, x2:1, xp2:ip};


  // non-designated compound literal
  struct fooB {int xB; int *xpB; int yB;};
  struct fooB fB;
  fB = (struct fooB) {1, ip, (int $tainted) 2};

  // non-designated compound literal
  struct foo2B {int x2B; int *xp2B; int y2B;};
  struct foo2B f2B;
  f2B = (struct foo2B) {y2B:(int $tainted) 2, x2B:1, xp2B:ip};


  // embedded int array and struct
  // Skip over vars that are methods/ctors/dtors
  struct foo3 {int x3; int y3;};
  struct foo4 {int x4; int y4;};
  struct gronk {
    struct foo3 f3;
    int z[3];
    struct foo4 f4;
  };
  struct gronk g = {
    1, (int $tainted) 2,        // f3
    3, (int $tainted) 4, 5,     // z
    (int $tainted) 6, 7,        // f4
  };
//    printf("gronk { f3 {x3:%d, y3:%d},\nz[0]:%d, z[1]:%d, z[2]:%d,\nf4 {x4:%d, y4:%d} }\n",
//           g.f3.x3, g.f3.y3,
//           g.z[0], g.z[1], g.z[2],
//           g.f4.x4, g.f4.y4);

  // embedded and nested int array and struct
  struct foo3b {int x3b; int y3b;};
  struct foo4b {int x4b; int y4b;};
  struct gronkb {
    struct foo3b f3b;
    int zb[3];
    struct foo4b f4b;
  };
  struct gronkb gb = {
    1, (int $tainted) 2,        // f3b
    {3, (int $tainted) 4, 5,},  // zb
    {(int $tainted) 6, 7,}      // f4b
  };

  // nested int array and struct; top level designated
  struct foo3c {int x3c; int y3c;};
  struct foo4c {int x4c; int y4c;};
  struct gronkc {
    struct foo3c f3c;
    int zc[3];
    struct foo4c f4c;
  };
  struct gronkc gc = {
    zc:{3, (int $tainted) 4, 5,},
    f4c:{(int $tainted) 6, 7,},
    f3c:{y3c:(int $tainted) 2, x3c:1},
  };

  // array with no size
  struct foo10d {int xd; int yd[];};
  struct foo10d f10d = {3, 4, (int $tainted) 5, 6};
  struct foo10e {int xe; int ye[]; int ze; int z2e;};
  struct foo10e f10e = {3, {4, (int $tainted) 5}, 6, (int $tainted) 7};
}
