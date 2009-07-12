// NOTE: This is not legal because I assign to const.
int main() {
  struct {
    int x;
  } const y;
  int const b = 1;
//    y.x = 3;                      // NOT LEGAL but not caught
  b = 2;                        // also NOT LEGAL
  return 0;
}
