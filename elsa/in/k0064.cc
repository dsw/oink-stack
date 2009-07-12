// functions overloaded by number of template parameters

// originally found in kde/kmainwindow.h

// k0064.cc:9:5: error: duplicate definition for `foo' of type `int ()()'; previous at k0064.cc:6:5 (from template; would be suppressed in permissive mode)
// typechecking results:
//   errors:   1
//   warnings: 0

// ERR-MATCH: error: duplicate definition for.*from template

template <class T1>
int foo() {}

template <class T1, class T2>
int foo() {}

int main()
{
  foo<int>();
  foo<int,int>();
}
