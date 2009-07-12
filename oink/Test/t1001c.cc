// atomic = *
int main() {
  void $untainted * x;
  int * $tainted * y;
  x = y;                        // bad
}
