// compoundinit2_1.c; see License.txt for copyright and terms of use

// testing nontrivial designated initializers
int main() {
  struct foo {
    int fooa;                   // (6)
    int foor;
    int foox;                   // (5)
    double fooy;
    int fooz[2];                // (1)
    double foow[6];             // (2)
    int fooq;
  };
  struct gronk {
    struct foo f1[6];
    int gronk1;
    struct {
      int x;
      double y;                 // (3)
      int z[6];                 // (4)
      int w;
    } f2;
    double gronk2;
    struct foo f3[6];
  };
  struct bar {
    struct gronk g[3];
    struct foo f;
    struct gronk g2;
  };

  struct bar b[6] = {
    [3].g[2].f1[2].fooz[1] = 1, 2.1, // (1)
    3.1,                        // should be foo.foow (2)
    [2].g[2].f2.y = 3.2,        // anon(f2).y (3)
    4, 5, 6, 7,                 // anon(f2).z (4)
    [4] = {                     // current_object == bar now
      .g2.f3 = {                // current_object == gronk.f3 now
        [2] = {
          .foox = 8             // foo.foox (5)
        }
      }
    },
    {
      .f = 9                    // bar[5].f, the first elt of which is fooa (6)
    }
  };
}
