// t0594.cc
// deduce template args in address of function where some
// arguments are explicitly provided

// related: t0592.cc, t0593.cc

template <typename T>
struct S {};

void bar(int (*pf)(int*, S<char> &));

template <typename T, typename U>
int foo(T*, S<U> &s);


void f()
{
  // The leading "0," is to remove a syntactic ambiguity.
  //
  // Here, the 'char' second argument to 'foo' is supposed to be
  // deduced, but Elsa complains that the number of template arguments
  // is wrong.
  0, bar(foo<int>);
}

// EOF
