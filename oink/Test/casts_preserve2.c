// CQUG 3.2.4 "By default, Cqual does not propagate qualifiers through
// type casts. . . . If you run Cqual with the flag -fcasts-preserve,
// however, then Cqual matches up qualifiers between the type cast to
// and the type cast from as much as possible. . . . if you cast to a
// type containing any user-specified qualifier, then the qualifiers
// will not propagate through that cast even if casts-preserve is
// enabled."

// Run this one with -fcasts-preserve and it should return "good".
int main() {
  $tainted int x;
  $untainted float y;
  y = (float $untainted) x;
}
