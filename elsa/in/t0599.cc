// in/t0599.cc
// demonstrate problem backpatching array sizes of template
// members, whether dependent or not

// extension of in/t0598.cc

// The problem here is that, while I adjust the type of A::x when I
// see its definition, when A gets instantiated as A<C>, I recompute
// the type of A<C>::x, losing the size info.  I don't instantiate
// out-of-line definitions until they are explicitly called for, but
// in the case of data members that are array types, perhaps I need to
// do so eagerly, or something like that.  It's complicated.

template <typename B>
class A {
public:
  static int x1[];
  static int x2[];
};

template <typename B>
int A<B>::x1[6];          // non-dependent size

template <typename B>
int A<B>::x2[B::y];       // dependent size

class C {
public:
  enum { y=6 };
};

void f()
{
  A<C> a;
  int z[6];               // expected type after instantiation
  __elsa_checkType(a.x1, z);
  __elsa_checkType(a.x2, z);
}

// EOF
