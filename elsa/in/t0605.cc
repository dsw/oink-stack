// t0605.cc
// overload template functions based on return type and template
// parameter list

// see cppstd 14.5.5.1p4

// needed for gcc-4 "bitset" header, bitset::to_string method

template <class T, class U>
class A {};


template <class T, class U>
A<T,U> makeA();

template <class T>
A<T,int> makeA();

A<int,int> makeA();


void f()
{
  makeA<float,float>();
  makeA<float>();
  makeA();
}


// EOF
