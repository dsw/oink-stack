// template parameterized by function pointer

// originally found in package 'aptitude'

// b.ii:4:1: error: cannot convert `bool (**)(int )' to `bool (*)(int )': different type constructors (inst from b.ii:7:24) (from template; would be suppressed in permissive mode)

// ERR-MATCH: cannot convert `.*?' to `.*?': different type constructors [(]inst from .*?[)] [(]from template; would be suppressed in permissive mode[)]

typedef bool (*mfunc)(int);
template<mfunc f> struct S
{
  int foo() {
    S<f> * s = new S<f>;
  }
};
