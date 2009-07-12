// template instantiated through inferred type instantiating another template
// with default parameter depending on first template parameter

// originally found in package 'lostirc_0.4.4-1'

// a.ii:16:8: error: could not evaluate default argument `S1<T2 /*anon*/>::MyType': attempt to extract member `MyType' from non-class `S1<T2 /*anon*/>' (from template; would be suppressed in permissive mode)
// a.ii:16:8: error: no argument supplied for template parameter `T3' (from template; would be suppressed in permissive mode)

// ERR-MATCH: could not evaluate default argument .*?: attempt to extract member .*? from non-class .*?

template <typename T1> 
struct S1 {
  typedef int MyType;
};

template <class T2, class T3 = typename S1<T2>::MyType>
struct S2
{
  S2(T2) {}
};

template <class T2> 
int foo(T2 a)
{
  S2<T2> s2(a);
}

int main()
{
  foo(42);
}
