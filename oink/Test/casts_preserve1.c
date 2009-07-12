// CQUG 3.2.4 "By default, Cqual does not propagate qualifiers through
// type casts. . . . If you run Cqual with the flag -fcasts-preserve,
// however, then Cqual matches up qualifiers between the type cast to
// and the type cast from as much as possible."

// Run this one with -fcasts-preserve and it should return "bad".
int main() {
  $tainted int x;
  $untainted float y;
  y = (float) x;
}
