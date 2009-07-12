// CQUG 3.2.4 "By default, Cqual does not propagate qualifiers through type casts."
int main() {
  $tainted int x;
  $untainted float y;           // good
  $untainted int y;             // bad
  y =
    (float)                     // good
    x;
}
