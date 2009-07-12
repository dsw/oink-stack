// t0630.cc
// implicit conversion from function to pointer-to-function in
// template argument list

// reduced from a testcase reported by Umesh Shankar

int f(int);

template <int F(int)>
class A {
};

A<f> Af;

// EOF
