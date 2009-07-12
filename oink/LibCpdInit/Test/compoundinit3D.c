// compoundinit3D.c; see License.txt for copyright and terms of use

// test a few more things with compound initializers
int main() {
  enum gronk {gronkA, gronkB, gronkC};

  struct foo4 {
    int y4[3];
    int z4;
  };
  struct foo4 f4 = {
    .y4[gronkC] =
    1,
    2
  };
}
