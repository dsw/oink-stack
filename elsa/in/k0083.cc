// friend template class declaration

// originally found in package 'lyx_1.3.4-2'

// a.ii:6:32: error: wrong # of template param lists in declaration of S1 (from template; would be suppressed in permissive mode)

// ERR-MATCH: wrong # of template param lists in declaration

template<typename T1> struct S1 {};

template<typename T1> struct S2 {
  template<typename T2> friend struct S1;
  int foo() {
    S1<bool> x;
  }
};

int main()
{
  S2<int> p;
  p.foo();
}
