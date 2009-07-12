// t0621.cc
// too eager to resolve DQTs during matching

template <class T>
class A {
public:
  typedef int INT;
};

template <class L, class R>
void f(A<L> const &, A<R> const &, typename A<L>::INT);

template <class LR>
void f(A<LR> const &, A<LR> const &, typename A<LR>::INT);

void g()
{
  A<int> a;

  // This requires overload resolution to compare the two declarations
  // of 'f'.  The first is more general, but Elsa ran into trouble
  // while comparing the DQTs, as it tried to resolve the one in the
  // pattern even when the concrete type was also a DQT.
  f(a,a, 3);
}


// ---------------
// same as above, this time with the DQT in the return type

namespace N {

template <class T>
class A {
public:
  typedef int INT;
};

template <class L, class R>
typename A<L>::INT f(A<L> const &, A<R> const &);

template <class LR>
typename A<LR>::INT f(A<LR> const &, A<LR> const &);

void g()
{
  A<int> a;

  f(a,a);
}

} // namespace N


// EOF
