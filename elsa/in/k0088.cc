// inferring const member function template parameter type

// see also simplified version: t0590.cc

// originally found in package 'fluxbox_0.9.11-1sarge0'

// b.ii:18:3: error: ambiguous overload; arguments:
//   1: int  (S::*)(/*m: struct S const & */ ) const
//  candidates:
//   b.ii:7:22: mem_fun_t<int /*anon*/, struct S const /*anon*/> mem_fun<int /*anon*/, struct S const /*anon*/>(int  (S::*__f)(/*m: struct S const & */ ) const)
//   b.ii:10:28: const_mem_fun_t<int /*anon*/, struct S /*anon*/> mem_fun<int /*anon*/, struct S /*anon*/>(int  (S::*__f)(/*m: struct S const & */ ) const)

// ERR-MATCH: ambiguous overload; arguments

template <class _Ret, class _Tp> class mem_fun_t {};
template <class _Ret, class _Tp> class const_mem_fun_t {};

template <class _Ret, class _Tp>
mem_fun_t<_Ret, _Tp> mem_fun(_Ret (_Tp::*__f)()) {}

template <class _Ret, class _Tp>
const_mem_fun_t<_Ret, _Tp> mem_fun(_Ret (_Tp::*__f)() const) {};

struct S {
  int foo() const {}
};

int main()
{
  mem_fun(&S::foo);
}
