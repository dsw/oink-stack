// t0627.cc
// call overloaded function with explicit address of template function

// isolated from t0628.cc

template <typename T>
class A {};


template <typename T>
int endl(A<T> &__os);


void f(int);
void f(int (*)(A<int> &));


void Baz()
{
  // the '&' here is the key
  f(&endl);
}

// EOF
