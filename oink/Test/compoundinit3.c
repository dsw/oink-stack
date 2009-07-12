// test a few more things with compound initializers
int main() {
  enum gronk {gronkA, gronkB, gronkC};

  {
    struct foo1 {
      int y1[3];                  // tainted
      int z1;                     // NOT tainted
    };
    struct foo1 f1 =
    {
      .y1[2] =
      (int $tainted) 0,
      0
    };
  }

  {
    struct foo2 {
      int y2[3];                  // NOT tainted
      int z2;                     // tainted
    };
    struct foo2 f2 = {
      .y2[2] =
      0,
      (int $tainted) 0
    };
  }

  {
    struct foo3 {
      int y3[3];                  // tainted
      int z3;                     // NOT tainted
    };
    struct foo3 f3 =
    {
      .y3[gronkC] =
      (int $tainted) 0,
      0
    };
  }

  {
    struct foo4 {
      int y4[3];                  // NOT tainted
      int z4;                     // tainted
    };
    struct foo4 f4 = {
      .y4[gronkC] =
      0,
      (int $tainted) 0
    };
  }
}
