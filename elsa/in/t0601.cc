// t0601.cc
// template template parameter for a function template

void eatFloat(float);

template <template <class U> class T>
int f(T<int> *p)
{
  T<float> tt;
  
  // TODO: Elsa thinks this is a non-dependent lookup?
  eatFloat(tt.x);

  return p->x;
}

template <class UU>
class A {
public:
  UU x;
};

void foo()
{
  A<int> a;
  0, f(&a);
}

// EOF
