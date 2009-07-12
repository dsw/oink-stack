// test that we can mark a variable as const when it is a parameter
// only

void f(int q, int $!const_when_param x, int y) {
  q = 3;                        // ok
  x = 4;                        // bad
  y = 5;                        // ok
}
