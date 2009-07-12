// t0593.cc
// problem with address of overloaded function where one is template
// variation of t0592.cc

template <typename T>
struct S {};

void bar(int (*pf)(S<char> &));

template<typename T>
int foo(S<T>& s)
{
  // b/c Elsa incorrectly chooses this 'foo', it will
  // instantiate it, and hence discover this error
  return T::nonexist;
}

int foo(S<char> &s);

void f()
{
  // should prefer the non-template
  bar(foo);
}

// EOF
