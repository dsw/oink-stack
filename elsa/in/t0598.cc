// in/t0598.cc
// from bug report by Alexander Vodomerov

// but see in/t0599.cc

template <typename B> 
class A {
public:
  static int x[];
};

template <typename B> 
int A<B>::x[B::y];

// EOF
