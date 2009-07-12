// this is in class basic_string which is in <string>; found by Karl,
// it turns out to cause an AST non-tree-ness, namely that the default
// argument 5 shows up in two different places.

template<typename T>
struct S1 {
  struct S2 {
    void foo(int arg = 5);
  };
};

template<typename T>
void S1<T>::S2::foo(int arg) {
}

int main()
{
  S1<int>::S2 s2;
  s2.foo();
}
