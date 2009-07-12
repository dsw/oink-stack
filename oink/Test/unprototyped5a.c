// (also checks 3277a461-899e-4381-a487-c2d0dad40a97)

typedef int (*func_t) ();
// extern int bar(func_t);

int foo () {
  int $tainted t;
  return t;                                         // BAD
}

int main()
{
  int $untainted u;
  u = bar(foo);
}
