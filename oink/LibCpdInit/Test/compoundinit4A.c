// compoundinit4A.c; see License.txt for copyright and terms of use

// test corner case of compound initializers and unions

// test that you can assign to a union and it assigns the first
// member; another way to look at it is as a test that you don't
// iterate through a union
void a() {
  union u1 {
    int x;
    int y;
  };
  struct s1 {
    int w;
    union u1 u;
    int z;
  };
  struct s1 s = {
    1,                          // w
    2,                          // x
    3,                          // NOTE: z, not y!
  };
}
