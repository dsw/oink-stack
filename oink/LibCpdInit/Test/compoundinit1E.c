// compoundinit1E.c; see License.txt for copyright and terms of use

// test compound initializers
int main() {
  // non-designated
  int *ip;// = 0;         // NULL
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
    1, 2,        // f3
    3, 4, 5,     // z
    6, 7,        // f4
  };
  //    printf("gronk { f3 {x3:%d, y3:%d},\nz[0]:%d, z[1]:%d, z[2]:%d,\nf4 {x4:%d, y4:%d} }\n",
  //           g.f3.x3, g.f3.y3,
  //           g.z[0], g.z[1], g.z[2],
  //           g.f4.x4, g.f4.y4);
}
