template <typename T>
struct S {
  int foo(int);
};

template <typename T>
int S<T>::foo(int i)
{
  return i;
}

template struct S<int>;
