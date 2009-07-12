// t0613.cc
// more complex version of t0612.cc

// reported by Umesh Shankar

// primary
template <class T>
struct B
{};

template <class U>
struct A
{};

template <class T, class U = B<T> >
class C {
public:
  typedef A<U> AU;

  AU find(int);
};

// This triggers the use of the 'B' primary for the value of the
// second template argument.  However, Elsa is not supposed to
// commit to a specialization because this is just a pointer.
C<int*> *c;

// explicit (partial) specialization
template <class T>
struct B<T*>
{};

void foo()
{
  // Here, Elsa recomputes the default argument, and this time finds
  // the explicit specialization.  Since the instantiation from the
  // primary is a different type than the instantiation from the
  // specialization, Elsa complains about a type incompatibility in
  // the initialization.
  C<int*>::AU x = c->find(3);
}

// EOF
