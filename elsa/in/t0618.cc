// t0618.cc
// commit to specialization too early; type used in a declaration
// of a return value, but that doesn't require it to be complete
// (until the function definition is provided)

// variation of t0612.cc

// from gcc-3.4.3 valarray header

template < class _Clos, typename _Tp >
class _Expr;

template < typename _Tp1, typename _Tp2 >
class _ValArray;

// primary
template < template < class, class > class _Meta,
           class _Dom >
class _GClos;

// declaration of a template that mentions _GClos<_ValArray, ...>
template < class _Tp >
class valarray
{
  _Expr < _GClos < _ValArray, _Tp >, _Tp > foo();
};

// instantiate valarray for int
valarray<int> _M_index;

// now provide the specialization for _ValArray
template < typename _Tp >
struct _GClos <_ValArray,  _Tp >
{
};

// EOF
