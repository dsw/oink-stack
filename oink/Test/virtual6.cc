

template<typename T>
struct S1
{
  virtual void foo();
};

template<typename T> void S1<T>::foo() {}

template<typename T>
struct S2 : S1<T>
{
  virtual void foo();
};

template<typename T> void S2<T>::foo() {}

int main() {
  S2<char> s2;
  // s2.foo();

  // S1<char> s1;
  // s1.foo();
}
