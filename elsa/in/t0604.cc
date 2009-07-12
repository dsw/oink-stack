// t0604.cc
// <valarray> header fragment: another DQT resolution issue

struct S {
  typedef int THETYPE;
};

template <class T>
class A {
public:
  template <class U>
  struct B {
    typedef typename U::THETYPE THETYPE2;
  };

  typename B<S>::THETYPE2 foo();
};

// When processing this return type, we look up B<S>::THETYPE2 in the
// scope of A.  But I was not specifying LF_ONLY_TYPES in the lookup
// flags, so the lookup routines decided that it must be the name of a
// variable, not a type.  LF_TYPENAME was also not specified because
// during DQT resolution I'm a long ways away from the syntax.  Once
// LF_ONLY_TYPES is specified, DQT resolution works properly.
//
// However, note that pretty printing this example reveals a printing
// flaw, namely that the return type of a template member may need to
// be qualified with the name of the template.
//
template <class T>
typename A<T>::template B<S>::THETYPE2    // return type
A<T>::foo()                               // function name + params
{
  return 0;
}

int main()
{
  A<int> a;
  return a.foo();
}

// EOF
