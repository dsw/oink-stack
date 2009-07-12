// test -fstrict-const; this will fail under strict const but
// otherwise
int main() {
  int
    const
    *a;
  int
//      $nonconst
    *b;
  b = a;
}
