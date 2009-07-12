// test corner case of compound initializers and unions

// test that you can assign to a union and it assigns the first
// member; another way to look at it is as a test that you don't
// iterate through a union
void a() {
  union u1 {
    int x;                      // tainted
    int y;                      // NOT tainted
  };
  struct s1 {
    int w;
    union u1 u;
    int z;                      // tainted
  };
  struct s1 s = {
    0,                          // w
    (int $tainted) 0,           // x
    (int $tainted) 0,           // NOTE: z, not y!
  };
}

// test you can use a designator to go down into a union
void b() {
  union u1b {
    int xb;
    int yb;                     // tainted
  };
  struct s1b {
    int wb;
    union u1b ub;
    int zb;
  };
  struct s1b sb = {
    .ub.yb = (int $tainted) 0,  // yb
  };
}
