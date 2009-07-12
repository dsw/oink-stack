// compoundinit3C.c; see License.txt for copyright and terms of use

// test a few more things with compound initializers
int main() {
  enum gronk {gronkA, gronkB, gronkC};

  struct foo3 {
    int y3[3];
    int z3;
  };
  struct foo3 f3 =
  {
    .y3[gronkC] =
    1,
    2
  };
}
