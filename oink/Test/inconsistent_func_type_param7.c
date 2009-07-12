// test taint flowing through inconsistent '...'

typedef int (*Func) (int a, int b);

int foo (int a, ...) {

  __builtin_va_list arg;
  __builtin_va_start(arg, a);

  int b = __builtin_va_arg(arg, int);

  __builtin_va_end(arg);

  return b;
}

int main()
{
  Func f = (Func) &foo;

  int $tainted t;
  int $untainted u;

  u = (*f)(0, t);
}
