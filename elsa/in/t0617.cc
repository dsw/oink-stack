// t0617.cc
// slightly more complex than t0616.cc

// haven't studied the issue here

namespace std
{
  template < class _Clos, typename _Tp > class _Expr;
  template < typename _Tp1, typename _Tp2 > class _ValArray;
  template < class _Oper, template < class, class > class _Meta1,
    template < class, class > class _Meta2, class _Dom1,
    class _Dom2 > class _BinClos;
  template < class _Tp > class valarray;
  template < typename _Tp1, typename _Tp2 > class _Constant;
  template < class _Oper, class _Clos > class _BinBase2
  {
  };
  template < class _Oper, class _Clos > class _BinBase1
  {
  };
  template < class _Oper, class _Dom > struct _BinClos <_Oper, _Constant,
    _Expr, typename _Dom::value_type, _Dom >:_BinBase1 < _Oper, _Dom >
  {
  };
  template < class _Oper, typename _Tp > struct _BinClos <_Oper, _ValArray,
    _Constant, _Tp, _Tp >:_BinBase2 < _Oper, valarray < _Tp > >
  {
  };
}

// EOF
