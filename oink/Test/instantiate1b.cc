namespace N {
  template <typename T>
  struct S {
    int foo(int x);
  };

  template <typename T>
  int S<T>::foo(int x) { return x; }

  template struct S<int>;
}
