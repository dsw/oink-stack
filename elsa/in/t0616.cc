// t0616.cc
// two explicit partial specializations of a template that accepts
// template template parameters

// from gcc-3.4.3 <valarray> header

// triggered a problem when I compare them for the purpose of
// detecting a violation of 14.7.3p6, but that comparison can
// also be reached by other paths in more complex examples

template < class _Clos, typename _Tp > 
class _Expr;

template < typename _Tp1, typename _Tp2 > 
class _ValArray;

// primary
template < template < class, class > class _Meta,
           class _Dom >
class _ValFunClos;

// specialization #1
template < class _Dom >
struct _ValFunClos <_Expr, _Dom >
{
};

// specialization #2
template < typename _Tp >
struct _ValFunClos <_ValArray, _Tp >
{
};

// EOF
