// template class with subclass whose member function with default argument is
// defined outside class.

// from <string>.
// originally found in package 'aptitude'.

// Assertion failed: !wasVisitedAST(obj), file cc.ast.gen.cc line 6948

// ERR-MATCH: Assertion failed:.* file cc.ast.gen.cc line 6948

template<typename T> struct S {
  struct Rep {
    void foo(int = 0);
  };
};

template<typename T> void S<T>::Rep:: foo(int x) {}

int main()
{
  S<int>::Rep r;
  r.foo();
}
