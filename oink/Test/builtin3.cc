// check that taint flows for builtins

int main()
{
  int $tainted t;
  int $untainted u;
  u = __builtin_putchar(t);     // BAD
}
