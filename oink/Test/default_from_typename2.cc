// ERR-MATCH: (0a00f982-9ebb-44e8-96cd-43b7db971fcb|e53e5e9b-b647-4ff2-a5ee-6be1a9ba5f11)

// from <vector>

template <typename T_> struct S2
{
  typedef T_ T;

  void foo (T t = T()) {}
};

int main()
{
  S2<int> x;
  x.foo();
}
