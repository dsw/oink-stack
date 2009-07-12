// testing nontrivial designated initializers
int main() {
  struct foo {
    int fooa;                   // tainted (6)
    int foor;
    int foox;                   // tainted (5)
    double fooy;
    int fooz[2];                // tainted (1)
    double foow[];              // tainted (2)
    int fooq;
  };
  struct gronk {
    struct foo f1[];
    int gronk1;
    struct {
      int x;
      double y;                 // tainted (3)
      int z[];                  // tainted (4)
      int w;
    } f2;
    double gronk2;
    struct foo f3[];
  };
  struct bar {
    struct gronk g[];
    struct foo f;
    struct gronk g2;
  };

  struct bar b[] = {
    [3].g[2].f1[2].fooz[1] = (int $tainted) 1, 2, // (1)
    (int $tainted) 3,           // should be foo.foow (2)
    [2].g[2].f2.y = (int $tainted) 3.2, // anon(f2).y (3)
    4, 5, 6, (int $tainted) 7,  // anon(f2).z (4)
    [4] = {                     // current_object == bar now
      .g2.f3 = {                // current_object == gronk.f3 now
        [2] = {
          .foox = (int $tainted) 18 // foo.foox (5)
        }
      }
    },
    {
      .f = (int $tainted) 9       // bar[5].f, the first elt of which is fooa (6)
    }
  };
}
