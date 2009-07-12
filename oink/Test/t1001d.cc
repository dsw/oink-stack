// atomic = *
// check when nesting levels don't match
int main() {
  void $untainted * x;
  int $tainted * * y;
  x = y;                        // bad
}
