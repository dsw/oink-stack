// t0609.cc
// experiments with operator precedence and associativity
// and bpprint printing

extern "C" void abort();

void check(int expr, int expected)
{
  if (expr != expected) {
    abort();
  }
}

int main()
{
  int a = 1;
  int b = 2;
  int c = 3;
  int d = 4;
  int e = 5;

  check(a - b + c, 2);
  check((a - b) + c, 2);
  check(a - (b + c), -4);
  
  check(a + b & d, 0);
  check((a + b) & d, 0);
  check(a + (b & d), 3);

  check(a & b + c, 1);
  check(a & (b + c), 1);
  check((a & b) + c, 3);

  // 'expected' is not correct here; I haven't bothered to calculate it
  check(a * b / c % d + e - a << b >> c < d > e <= a >= b, 0);
  check(((((((((((a * b) / c) % d) + e) - a) << b) >> c) < d) > e) <= a) >= b, 0);
  check(a * (b / (c % (d + (e - (a << (b >> (c < (d > (e <= (a >= b)))))))))), 0);
  
  check((a, b), 2);
  check((a, b, c), 3);
  check((a, (b, c)), 3);

  return 0;
}


// EOF
