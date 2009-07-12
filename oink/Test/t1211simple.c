int main() {
  int $tainted x;
  int $untainted y;
  int q;
  int r;
  int z;
  q = x;
  y = q;                        // bad
  q = y;                        // good
  r = q;
  y = z;
  z = y;
}
