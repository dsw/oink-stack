// test that a Declarator correctly initializes a non-compound type,
// here an int, within an IN_ctor

int $tainted i0;

int main() {
  int i1(i0);                   // an IN_ctor for a non-compound type
  int $untainted u1;
  u1 = i1;
  return 0;
}
