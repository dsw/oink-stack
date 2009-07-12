int main() {
  int $tainted x;
  int $untainted y;
  int q;
  q = x;
  y = q;                        // bad
  q = y;                        // good
}
