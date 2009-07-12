// t0586.cc
// from oink/Test/outlined_template_dtor1.cc

// reflected as an Elsa test with some template hackery

template <class T>
struct A {
    A();
    ~A();
};

template <class T>
class B {};

template <class T>
A<T>::~A()
{
  // The purpose of this line is to ensure that A<T>::~A is
  // instantiated.  If it is, then when we uncomment the following
  // line, it will cause a compilation error.
  //ERROR(1): 1 + B<T>::nonexist;
}


template <class T>
A<T>::A()
{
}

int main()
{
    A<int> a;
    //a.~A();    // this is implicit, essentially
}
