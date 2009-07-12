typedef int INT;

int main() {
  float                         // good
  INT                           // bad
    $tainted * x;
  int $untainted * y;

  void * q;
  q = x;
  y = q;
}
