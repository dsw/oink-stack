#ifdef __GNUC__
#define $tainted
#define $untainted
#endif

int foo()
{
  int $tainted t = 2;
  return t;
}
