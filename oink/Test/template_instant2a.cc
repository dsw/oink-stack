
// template_instant1b.cc contains the definition for S<T>::foo and also a
// template instantiation: template struct S<int>;

// we better link properly!

template <typename T>
struct S {
  int foo(int);
};

int main()
{
  S<int> s;

  int $tainted t;
  int $untainted u;
  u = s.foo(t);
}
