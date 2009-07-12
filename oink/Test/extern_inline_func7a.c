extern int foo();

extern inline int foo()
{
}

int foo()
{
  int $tainted t;
  return t;
}

int main()
{
  int $untainted u;
  u = foo();
  return u;
}
