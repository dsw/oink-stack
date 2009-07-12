// t0597.cc
// very basic template template parameter example

template <class T>
class A {
public:
  T t;
};

template <template <class> class U>
class B {
public:
  U<int> u;
};

void f(int);

void g()
{
  (B<A>*)0;

  B<A> b;
  f(b.u.t);

  int i;
  __elsa_checkType(b.u.t, i);
}


// EOF
