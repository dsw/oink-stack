
typedef int (*f)(int);
typedef int (*g)(int, int);

int G1(int a, int b) {
  return b;
}

int main()
{
  int $tainted t;
  int $untainted u;

  g g1;

  g1 = (g) (f) G1;

  u = g1(0, t);                 // BAD
}
