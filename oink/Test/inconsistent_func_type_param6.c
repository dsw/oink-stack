typedef int (*Func) (int a, int b);

int foo (int a, ...) {
  return a;
}

int main()
{
  Func f = (Func) &foo;

  int $tainted t;
  int $untainted u;

  u = (*f)(t);
}
