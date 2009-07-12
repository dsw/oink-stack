// t0608.cc
// member of a template class invokves a template member of that class

// happens in gcc-4 bitset header

template <class W>
class C {
public:
  template <class T, class U>
  int foo();

  // inline defn
  void bar()
  {
    foo<int, int>();
  }

  void baz();
};
                  
// out-of-line defn
template <class W>
void C<W>::baz()
{
  foo<int, int>();
}


void f()
{
  C<int> c;
  c.bar();
  c.baz();
}


// EOF
