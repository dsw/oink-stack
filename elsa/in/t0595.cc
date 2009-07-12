// t0595.cc
// several problems with address of overloaded template function

// variation of t0592.cc


template <typename T>
struct S {};

void bar(int (*pf)(S<char> &, int));


// The correct resolution is this one because, at the time the
// definition of X::f is seen, this is the only 'foo' that has been
// declared, and 'foo' is not dependent on X's template args.
// However, Elsa will erroneously conclude that it *is* dependent, due
// to 'foo' involving abstract types.
template <typename T, typename U>
int foo(S<T> &s, U u);


template <class V>
struct X {
  void f()
  {
    0, bar(foo);
  }
};


// Due to Elsa's incorrect conclusion about 'foo' being dependent, it
// will delay lookup until the instantiation site, at which point this
// 'foo' is also visible, and is more specialized.  However, because
// we don't do proper resolution anyway, we still pick the above
// function for now.
template <typename T>
int foo(S<T> &s, int)
{
  return T::nonexist;
}


// And, somehow this triggers an assertion failure:
//    Assertion failed: !var->isTemplate() && 
//    "ee42ebc5-7154-4ace-be35-c2090a2821c5", file cc_ast_aux.cc line 1578
// So the whole thing is a mess.  :(
template void X<int>::f();


// EOF
