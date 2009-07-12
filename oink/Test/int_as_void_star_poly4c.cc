// test that void * and int qualifiers are shared even with pointer
// arithmetic

int main() {
  int $tainted v;
  int v2;
  void * q;
  q = (void*) v2;               // good
  // note the way cast distributes over subtraction
  q = ((void*)v) - ((void*)v2); // bad
  void * $untainted s = q;
}
