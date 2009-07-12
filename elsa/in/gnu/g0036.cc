// g0036.cc
// from <complex> header

inline __complex__ float
__complex_log(__complex__ float __z) { return __builtin_clogf(__z); }

inline __complex__ double
__complex_log(__complex__ double __z) { return __builtin_clog(__z); }

inline __complex__ long double
__complex_log(const __complex__ long double& __z)
{ return __builtin_clogl(__z); }

// EOF
