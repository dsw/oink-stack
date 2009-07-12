// test taint flowing through inconsistent '...'

typedef int (*Func) (int a, ...);

int foo (int a, int b) {
  return b;
}

int main()
{
  Func f = (Func) &foo;

  int $tainted t;
  int $untainted u;

  u = (*f)(0, t);
}
