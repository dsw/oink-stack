namespace N {
  template <typename T>
  struct S {
    int foo(int x);
  };
}

#if !defined QUAL
#define $tainted
#define $untainted
#endif

int main()
{
  N::S<int> s;
  int $tainted t;
  int $untainted u;
  u = s.foo(t);
}
