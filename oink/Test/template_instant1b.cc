template <typename T>
struct S {
  void foo();
};

template <typename T>
void S<T>::foo()
{
}

template struct S<int>;
