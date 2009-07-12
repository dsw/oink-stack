//  * = *
int main() {
  int $untainted * x;
  int * $tainted y;
  x = y;                        // good
  *x = y;                       // bad
}
