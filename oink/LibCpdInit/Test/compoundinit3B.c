// compoundinit3B.c; see License.txt for copyright and terms of use

// test a few more things with compound initializers
int main() {
  enum gronk {gronkA, gronkB, gronkC};

  struct foo2 {
    int y2[3];
    int z2;
  };
  struct foo2 f2 = {
    .y2[2] =
    1,
    2
  };
}
