// t0606.cc
// overload based on template parameter list alone

// see 14.5.5.1p4

// related to t0605.cc

template <class T, class U>
class A {};


template <class T, class U>
A<T,U> anotherA();

template <class T, class U, class V>
A<T,U> anotherA();

void g()
{
  anotherA<float,float>();
  anotherA<float,float,float>();
}


// EOF
