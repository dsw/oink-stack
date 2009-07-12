// test that void * and int qualifiers are shared

int main() {
  void * $tainted v;
  int q;
  q = v;                        // bad
  int $untainted s = (2 + q + 7);
}
