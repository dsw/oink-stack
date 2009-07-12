// test that void * and int qualifiers are shared even with pointer
// arithmetic

int main() {
  void * $tainted v;
  void * v2;
  int q;
  q = (int) v2;                 // good
  // note the way cast distributes over subtraction
  q = ((int)v) - ((int)v2);     // bad
  int $untainted s = (2 + q + 7);
}
