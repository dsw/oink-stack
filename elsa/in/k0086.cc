// enable_if: template argument expressions

// originally found in package 'cimg_1.0.7-1'

// in code near a.ii:14:3:
// unimplemented: template.cc:3961: applyArgumentMap: dep-expr is not E_variable

// ERR-MATCH: unimplemented: .* applyArgumentMap: dep-expr is not E_variable

double atan2(double y, double x)
{}

template<typename, bool>
struct enable_if {};

template<typename _Tp>
struct is_integer {};

template<typename _Tp, typename _Up>                   // template params
enable_if<double, is_integer<_Tp>::_M_type && false>   // rettype
atan2(_Tp y, _Up)                                      // name+params
{}                                                     // body

int main()
{
  atan2(0,0);
}

