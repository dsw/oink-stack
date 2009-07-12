#ifdef __GNUC__
#define $tainted
#define $untainted
#endif

extern int foo();

extern inline int foo()
{
  int $tainted t = 1;
  return t;
}

int main()
{
  int $untainted u;
  u = foo();
  return u;
}
