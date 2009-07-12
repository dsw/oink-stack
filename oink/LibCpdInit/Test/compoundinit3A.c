// compoundinit3A.c; see License.txt for copyright and terms of use

// test a few more things with compound initializers
int main() {
  enum gronk {gronkA, gronkB, gronkC};

  struct foo1 {
    int y1[3];
    int z1;
  };
  struct foo1 f1 =
  {
    .y1[2] =
    1,
    2
  };
}
