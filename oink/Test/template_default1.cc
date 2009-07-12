// ERR-MATCH: 3391b39c-5527-4546-a0f1-dc2cbff50f64

template <typename T> struct S1 {
  void foo(float = 0);
};

int main()
{
  S1<char> s1;
}
