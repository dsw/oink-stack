// template-parameter-instantiated template class without 'typename' in
// constructor initializer

// originally found in package 'gnomemeeting_1.2.1-1'

// a.ii:14:17: error: `S1<T1 /*anon*/>::C1' does not denote any class (from template; would be suppressed in permissive mode)

// ERR-MATCH: `.*?<.*?>::.*?' does not denote any class

template <class T1>
struct S1
{
  struct C1 {};
};

template <class T1>
struct S2 : S1<T1>
{
  struct C2 : S1<T1>::C1
  {
    C2() : S1<T1>::C1()
    {
      typename S1<T1>::C1 c1;
    }
  };
};

int main()
{
  S2<int> s2;
}
