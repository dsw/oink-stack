int main()
{
  int $tainted t;
  int $untainted u;
  u = __builtin_expect(t, 1);   // BAD
}
