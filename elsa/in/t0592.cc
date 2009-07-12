// t0592.cc
// simplified version of in/k0105.cc

template <typename T>
struct S {};

void bar(void (*pf)(S<char> &));

template<typename T>
void foo(S<T>& s);

void f()
{
  // must resolve address of template function; cppstd 14.8.2.2
  bar(foo);
}


// further variation: infer two template arguments

void bar2(void (*pf)(int*, S<char> &));

template <typename T, typename U>
void foo2(U*, S<T>& s);

void f2()
{
  bar2(foo2);
}


// EOF
