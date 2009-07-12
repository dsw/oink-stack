// t0603.cc
// isolated fragment from <valarray> header

struct A {};

template <class T>
struct B {

  template <class U>
  struct C {
    typedef U SomeType;
  };

  // Elsa complains:
  // Assertion failed: !argIter.isDone(), file template.cc line 1775
  //
  // The problem is Elsa is expecting that when C is instantiated
  // there will be two arguments, one for B and one for C, but here
  // in the scope of B there is only an argument for C.
  //
  // The solution for the moment is to call C<A> "dependent", even
  // though technically it is not (I think).
  //
  typename C<A>::SomeType foo();
};

A returnsA();

void f()
{
  B<int> b;
  __elsa_checkType(b.foo(), returnsA());
}



// ---------------
// as above, but a non-template layer in between BB and C
template <class T>
struct BB {

  struct NonTemplate {

    template <class U>
    struct C {
      typedef U SomeType;
    };

    typename C<A>::SomeType foo();

  };
};

void ff()
{
  BB<int>::NonTemplate b;
  __elsa_checkType(b.foo(), returnsA());
}
