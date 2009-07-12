// compoundinit4B.c; see License.txt for copyright and terms of use

// test corner case of compound initializers and unions

// test you can use a designator to go down into a union
void b() {
  union u1b {
    int xb;
    int yb;
  };
  struct s1b {
    int wb;
    union u1b ub;
    int zb;
  };
  struct s1b sb = {
    .ub.yb = 1,                 // yb
  };
}
