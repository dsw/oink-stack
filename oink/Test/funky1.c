// test leaf polymorphism and $_ notation.

int $_1 gronk(int $_1_2 * w, int $_1 * $_1_3 x, int y, int $_1_28 z);

// hook the edges up in the same way as with gronk
int gronk2(int * w, int * x, int y, int z) {
  *w = *x;
  x = *x;                       // ridiculous, I know
  z = *x;
  return *x;
}

int main() {
  {
    int * w0;
    int $tainted * x0;
    int $untainted y0;
    int z0;
    int r0;
    r0 = gronk(w0, x0, y0, z0);
  }
  {
    int * w0;
    int * x0;
    int y0;
    int z0;
    int r0;
    r0 = gronk(w0, x0, y0, z0);
  }
  {
    int * w0;
    int $tainted * x0;
    int $untainted y0;
    int z0;
    int r0;
    r0 = gronk2(w0, x0, y0, z0);
  }
  {
    int * w0;
    int * x0;
    int y0;
    int z0;
    int r0;
    r0 = gronk2(w0, x0, y0, z0);
  }
}
