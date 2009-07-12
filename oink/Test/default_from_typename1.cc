// ERR-MATCH: 1d5fad1c-35f6-4c6c-9262-f8b8a0a22875

// from <vector>

template <typename T_> struct S1 {
  typedef T_ T;
};

template <typename T_> struct S2 : S1<T_>
{
  typedef typename S1<T_>::T T;

  void foo (T t = T()) {}
};

int main()
{
  S2<int> x;
  x.foo();
}
