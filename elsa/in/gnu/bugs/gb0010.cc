// gb0010.cc
// use 'class' when inappropriate

struct A {
  typedef int INT;
};

// GCC and ICC reject this
//ERROR(1): class A::INT x;
  

template <class T>
struct B {
  typedef int INT;
};

// they also reject this
//ERROR(2): class B<int>::INT y;


template <class T>
void f(T)
{
  // GCC allows this, however
  class B<T>::INT z;
}

// even when instantiated
template void f(int);


