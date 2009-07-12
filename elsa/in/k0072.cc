// template <class T> void foo(S1<T> s1, const typename T::T2 t2) {}

// originally found in package 'aspell'

// ERR-MATCH: Assertion failed: .*ceae1527-94a7-480d-9134-5dbd8cbfb2aa

template <class T>
struct S1 {};

template <class T>
void foo(S1<T> s1, const typename T::T2 t2) {}

struct A {
  typedef int T2;
};

void g()
{
  S1<A> s1a;
  foo(s1a, 3);
}
