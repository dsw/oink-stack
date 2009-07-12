// t0587.cc
// Elsa version of oink/Test/template_impl_addrof_func1.cc

template <typename T>
struct S1 {
  static void foo();
};

template <class T>
class B {};

template <typename T>
void S1<T>::foo()
{
  // this function should be instantiated, meaning when the following
  // line is uncommented, we detect the error
  //ERROR(1): 1 + B<T>::nonexist;
}

typedef void (*func)(void);

int main()
{
  func f = /* missing "&" */ S1<int>::foo;
}
