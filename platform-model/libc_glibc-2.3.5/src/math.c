#include <glibc-fs/prelude.h>
#include <math.h>

FAKEBUILTIN
double acos (double x) {}
double __acos (double x) {}

FAKEBUILTIN
double asin (double x) {}
double __asin (double x) {}

FAKEBUILTIN
double atan (double x) {}
double __atan (double x) {}

FAKEBUILTIN
double atan2 (double y, double x) {}
double __atan2 (double y, double x) {}

FAKEBUILTIN
double cos (double x) {}
double __cos (double x) {}

FAKEBUILTIN
double sin (double x) {}
double __sin (double x) {}

FAKEBUILTIN
double tan (double x) {}
double __tan (double x) {}

FAKEBUILTIN
double cosh (double x) {}
double __cosh (double x) {}

FAKEBUILTIN
double sinh (double x) {}
double __sinh (double x) {}

FAKEBUILTIN
double tanh (double x) {}
double __tanh (double x) {}

FAKEBUILTIN
void sincos (double x, double *sinx, double *cosx) {}
void __sincos (double x, double *sinx, double *cosx) {}

FAKEBUILTIN
double acosh (double x) {}
double __acosh (double x) {}

FAKEBUILTIN
double asinh (double x) {}
double __asinh (double x) {}

FAKEBUILTIN
double atanh (double x) {}
double __atanh (double x) {}

FAKEBUILTIN
double exp (double x) {}
double __exp (double x) {}

FAKEBUILTIN
double frexp (double x, int *exponent) {}
double __frexp (double x, int *exponent) {}

FAKEBUILTIN
double ldexp (double x, int exponent) {}
double __ldexp (double x, int exponent) {}

FAKEBUILTIN
double log (double x) {}
double __log (double x) {}

FAKEBUILTIN
double log10 (double x) {}
double __log10 (double x) {}

FAKEBUILTIN
double modf (double x, double *iptr) {}
double __modf (double x, double *iptr) {}

FAKEBUILTIN
double exp10 (double x) {}
double __exp10 (double x) {}

FAKEBUILTIN
double pow10 (double x) {}
double __pow10 (double x) {}

FAKEBUILTIN
double expm1 (double x) {}
double __expm1 (double x) {}

FAKEBUILTIN
double log1p (double x) {}
double __log1p (double x) {}

FAKEBUILTIN
double logb (double x) {}
double __logb (double x) {}

FAKEBUILTIN
double exp2 (double x) {}
double __exp2 (double x) {}

FAKEBUILTIN
double log2 (double x) {}
double __log2 (double x) {}

FAKEBUILTIN
double pow (double x, double y) {}
double __pow (double x, double y) {}

FAKEBUILTIN
double sqrt (double x) {}
double __sqrt (double x) {}

FAKEBUILTIN
double hypot (double x, double y) {}
double __hypot (double x, double y) {}

FAKEBUILTIN
double cbrt (double x) {}
double __cbrt (double x) {}

FAKEBUILTIN
double ceil (double x) {}
double __ceil (double x) {}

FAKEBUILTIN
double fabs (double x) {}
double __fabs (double x) {}

FAKEBUILTIN
double floor (double x) {}
double __floor (double x) {}

FAKEBUILTIN
double fmod (double x, double y) {}
double __fmod (double x, double y) {}

int __isinf (double value) {}

int __finite (double value) {}

#undef isinf
int isinf (double value) {}

int finite (double value) {}

FAKEBUILTIN
double drem (double x, double y) {}
double __drem (double x, double y) {}

FAKEBUILTIN
double significand (double x) {}
double __significand (double x) {}

FAKEBUILTIN
double copysign (double x, double y) {}
double __copysign (double x, double y) {}

FAKEBUILTIN
double nan (const char *tagb) {}
double __nan (const char *tagb) {}

int __isnan (double value) {}

#undef isnan
int isnan (double value) {}

FAKEBUILTIN
double j0 (double x) {}
double __j0 (double x) {}
FAKEBUILTIN
double j1 (double x) {}
double __j1 (double x) {}
FAKEBUILTIN
double jn (int n, double x) {}
double __jn (int n, double x) {}
FAKEBUILTIN
double y0 (double x) {}
double __y0 (double x) {}
FAKEBUILTIN
double y1 (double x) {}
double __y1 (double x) {}
FAKEBUILTIN
double yn (int n, double x) {}
double __yn (int n, double x) {}

FAKEBUILTIN
double erf (double x) {}
double __erf (double x) {}
FAKEBUILTIN
double erfc (double x) {}
double __erfc (double x) {}
FAKEBUILTIN
double lgamma (double x) {}
double __lgamma (double x) {}

FAKEBUILTIN
double tgamma (double x) {}
double __tgamma (double x) {}

FAKEBUILTIN
double gamma (double x) {}
double __gamma (double x) {}

double lgamma_r (double d, int *signgamp) {}
double __lgamma_r (double d, int *signgamp) {}

FAKEBUILTIN
double rint (double x) {}
double __rint (double x) {}

FAKEBUILTIN
double nextafter (double x, double y) {}
double __nextafter (double x, double y) {}

FAKEBUILTIN
double nexttoward (double x, long double y) {}
double __nexttoward (double x, long double y) {}

FAKEBUILTIN
double remainder (double x, double y) {}
double __remainder (double x, double y) {}

FAKEBUILTIN
double scalbn (double x, int n) {}
double __scalbn (double x, int n) {}

FAKEBUILTIN
int ilogb (double x) {}
int __ilogb (double x) {}

FAKEBUILTIN
double scalbln (double x, long int n) {}
double __scalbln (double x, long int n) {}

FAKEBUILTIN
double nearbyint (double x) {}
double __nearbyint (double x) {}

FAKEBUILTIN
double round (double x) {}
double __round (double x) {}

FAKEBUILTIN
double trunc (double x) {}
double __trunc (double x) {}

FAKEBUILTIN
double remquo (double x, double y, int *quo) {}
double __remquo (double x, double y, int *quo) {}

FAKEBUILTIN
long int lrint (double x) {}
long int __lrint (double x) {}
FAKEBUILTIN
long long int llrint (double x) {}
long long int __llrint (double x) {}

FAKEBUILTIN
long int lround (double x) {}
long int __lround (double x) {}
FAKEBUILTIN
long long int llround (double x) {}
long long int __llround (double x) {}

FAKEBUILTIN
double fdim (double x, double y) {}
double __fdim (double x, double y) {}

FAKEBUILTIN
double fmax (double x, double y) {}
double __fmax (double x, double y) {}

FAKEBUILTIN
double fmin (double x, double y) {}
double __fmin (double x, double y) {}

int __fpclassify (double value) {}

int __signbit (double value) {}

FAKEBUILTIN
double fma (double x, double y, double z) {}
double __fma (double x, double y, double z) {}

FAKEBUILTIN
double scalb (double x, double n) {}
double __scalb (double x, double n) {}

FAKEBUILTIN
float acosf (float x) {}
float __acosf (float x) {}

FAKEBUILTIN
float asinf (float x) {}
float __asinf (float x) {}

FAKEBUILTIN
float atanf (float x) {}
float __atanf (float x) {}

FAKEBUILTIN
float atan2f (float y, float x) {}
float __atan2f (float y, float x) {}

FAKEBUILTIN
float cosf (float x) {}
float __cosf (float x) {}

FAKEBUILTIN
float sinf (float x) {}
float __sinf (float x) {}

FAKEBUILTIN
float tanf (float x) {}
float __tanf (float x) {}

FAKEBUILTIN
float coshf (float x) {}
float __coshf (float x) {}

FAKEBUILTIN
float sinhf (float x) {}
float __sinhf (float x) {}

FAKEBUILTIN
float tanhf (float x) {}
float __tanhf (float x) {}

FAKEBUILTIN
void sincosf (float x, float *sinx, float *cosx) {}
void __sincosf (float x, float *sinx, float *cosx) {}

FAKEBUILTIN
float acoshf (float x) {}
float __acoshf (float x) {}

FAKEBUILTIN
float asinhf (float x) {}
float __asinhf (float x) {}

FAKEBUILTIN
float atanhf (float x) {}
float __atanhf (float x) {}

FAKEBUILTIN
float expf (float x) {}
float __expf (float x) {}

FAKEBUILTIN
float frexpf (float x, int *exponent) {}
float __frexpf (float x, int *exponent) {}

FAKEBUILTIN
float ldexpf (float x, int exponent) {}
float __ldexpf (float x, int exponent) {}

FAKEBUILTIN
float logf (float x) {}
float __logf (float x) {}

FAKEBUILTIN
float log10f (float x) {}
float __log10f (float x) {}

FAKEBUILTIN
float modff (float x, float *iptr) {}
float __modff (float x, float *iptr) {}

FAKEBUILTIN
float exp10f (float x) {}
float __exp10f (float x) {}

FAKEBUILTIN
float pow10f (float x) {}
float __pow10f (float x) {}

FAKEBUILTIN
float expm1f (float x) {}
float __expm1f (float x) {}

FAKEBUILTIN
float log1pf (float x) {}
float __log1pf (float x) {}

FAKEBUILTIN
float logbf (float x) {}
float __logbf (float x) {}

FAKEBUILTIN
float exp2f (float x) {}
float __exp2f (float x) {}

FAKEBUILTIN
float log2f (float x) {}
float __log2f (float x) {}

FAKEBUILTIN
float powf (float x, float y) {}
float __powf (float x, float y) {}

FAKEBUILTIN
float sqrtf (float x) {}
float __sqrtf (float x) {}

FAKEBUILTIN
float hypotf (float x, float y) {}
float __hypotf (float x, float y) {}

FAKEBUILTIN
float cbrtf (float x) {}
float __cbrtf (float x) {}

FAKEBUILTIN
float ceilf (float x) {}
float __ceilf (float x) {}

FAKEBUILTIN
float fabsf (float x) {}
float __fabsf (float x) {}

FAKEBUILTIN
float floorf (float x) {}
float __floorf (float x) {}

FAKEBUILTIN
float fmodf (float x, float y) {}
float __fmodf (float x, float y) {}

int __isinff (float value) {}

int __finitef (float value) {}

int isinff (float value) {}

int finitef (float value) {}

FAKEBUILTIN
float dremf (float x, float y) {}
float __dremf (float x, float y) {}

FAKEBUILTIN
float significandf (float x) {}
float __significandf (float x) {}

FAKEBUILTIN
float copysignf (float x, float y) {}
float __copysignf (float x, float y) {}

FAKEBUILTIN
float nanf (const char *tagb) {}
float __nanf (const char *tagb) {}

int __isnanf (float value) {}

int isnanf (float value) {}

FAKEBUILTIN
float j0f (float x) {}
float __j0f (float x) {}
FAKEBUILTIN
float j1f (float x) {}
float __j1f (float x) {}
FAKEBUILTIN
float jnf (int n, float x) {}
float __jnf (int n, float x) {}
FAKEBUILTIN
float y0f (float x) {}
float __y0f (float x) {}
FAKEBUILTIN
float y1f (float x) {}
float __y1f (float x) {}
FAKEBUILTIN
float ynf (int n, float x) {}
float __ynf (int n, float x) {}

FAKEBUILTIN
float erff (float x) {}
float __erff (float x) {}
FAKEBUILTIN
float erfcf (float x) {}
float __erfcf (float x) {}
FAKEBUILTIN
float lgammaf (float x) {}
float __lgammaf (float x) {}

FAKEBUILTIN
float tgammaf (float x) {}
float __tgammaf (float x) {}

FAKEBUILTIN
float gammaf (float x) {}
float __gammaf (float x) {}

float lgammaf_r (float f, int *signgamp) {}
float __lgammaf_r (float f, int *signgamp) {}

FAKEBUILTIN
float rintf (float x) {}
float __rintf (float x) {}

FAKEBUILTIN
float nextafterf (float x, float y) {}
float __nextafterf (float x, float y) {}

FAKEBUILTIN
float nexttowardf (float x, long double y) {}
float __nexttowardf (float x, long double y) {}

FAKEBUILTIN
float remainderf (float x, float y) {}
float __remainderf (float x, float y) {}

FAKEBUILTIN
float scalbnf (float x, int n) {}
float __scalbnf (float x, int n) {}

FAKEBUILTIN
int ilogbf (float x) {}
int __ilogbf (float x) {}

FAKEBUILTIN
float scalblnf (float x, long int n) {}
float __scalblnf (float x, long int n) {}

FAKEBUILTIN
float nearbyintf (float x) {}
float __nearbyintf (float x) {}

FAKEBUILTIN
float roundf (float x) {}
float __roundf (float x) {}

FAKEBUILTIN
float truncf (float x) {}
float __truncf (float x) {}

FAKEBUILTIN
float remquof (float x, float y, int *quo) {}
float __remquof (float x, float y, int *quo) {}

FAKEBUILTIN
long int lrintf (float x) {}
long int __lrintf (float x) {}
FAKEBUILTIN
long long int llrintf (float x) {}
long long int __llrintf (float x) {}

FAKEBUILTIN
long int lroundf (float x) {}
long int __lroundf (float x) {}
FAKEBUILTIN
long long int llroundf (float x) {}
long long int __llroundf (float x) {}

FAKEBUILTIN
float fdimf (float x, float y) {}
float __fdimf (float x, float y) {}

FAKEBUILTIN
float fmaxf (float x, float y) {}
float __fmaxf (float x, float y) {}

FAKEBUILTIN
float fminf (float x, float y) {}
float __fminf (float x, float y) {}

int __fpclassifyf (float value) {}

int __signbitf (float value) {}

FAKEBUILTIN
float fmaf (float x, float y, float z) {}
float __fmaf (float x, float y, float z) {}

FAKEBUILTIN
float scalbf (float x, float n) {}
float __scalbf (float x, float n) {}

FAKEBUILTIN
long double acosl (long double x) {}
long double __acosl (long double x) {}

FAKEBUILTIN
long double asinl (long double x) {}
long double __asinl (long double x) {}

FAKEBUILTIN
long double atanl (long double x) {}
long double __atanl (long double x) {}

FAKEBUILTIN
long double atan2l (long double y, long double x) {}
long double __atan2l (long double y, long double x) {}

FAKEBUILTIN
long double cosl (long double x) {}
long double __cosl (long double x) {}

FAKEBUILTIN
long double sinl (long double x) {}
long double __sinl (long double x) {}

FAKEBUILTIN
long double tanl (long double x) {}
long double __tanl (long double x) {}

FAKEBUILTIN
long double coshl (long double x) {}
long double __coshl (long double x) {}

FAKEBUILTIN
long double sinhl (long double x) {}
long double __sinhl (long double x) {}

FAKEBUILTIN
long double tanhl (long double x) {}
long double __tanhl (long double x) {}

FAKEBUILTIN
void sincosl (long double x, long double *sinx, long double *cosx) {}
void __sincosl (long double x, long double *sinx, long double *cosx) {}

FAKEBUILTIN
long double acoshl (long double x) {}
long double __acoshl (long double x) {}

FAKEBUILTIN
long double asinhl (long double x) {}
long double __asinhl (long double x) {}

FAKEBUILTIN
long double atanhl (long double x) {}
long double __atanhl (long double x) {}

FAKEBUILTIN
long double expl (long double x) {}
long double __expl (long double x) {}

FAKEBUILTIN
long double frexpl (long double x, int *exponent) {}
long double __frexpl (long double x, int *exponent) {}

FAKEBUILTIN
long double ldexpl (long double x, int exponent) {}
long double __ldexpl (long double x, int exponent) {}

FAKEBUILTIN
long double logl (long double x) {}
long double __logl (long double x) {}

FAKEBUILTIN
long double log10l (long double x) {}
long double __log10l (long double x) {}

FAKEBUILTIN
long double modfl (long double x, long double *iptr) {}
long double __modfl (long double x, long double *iptr) {}

FAKEBUILTIN
long double exp10l (long double x) {}
long double __exp10l (long double x) {}

FAKEBUILTIN
long double pow10l (long double x) {}
long double __pow10l (long double x) {}

FAKEBUILTIN
long double expm1l (long double x) {}
long double __expm1l (long double x) {}

FAKEBUILTIN
long double log1pl (long double x) {}
long double __log1pl (long double x) {}

FAKEBUILTIN
long double logbl (long double x) {}
long double __logbl (long double x) {}

FAKEBUILTIN
long double exp2l (long double x) {}
long double __exp2l (long double x) {}

FAKEBUILTIN
long double log2l (long double x) {}
long double __log2l (long double x) {}

FAKEBUILTIN
long double powl (long double x, long double y) {}
long double __powl (long double x, long double y) {}

FAKEBUILTIN
long double sqrtl (long double x) {}
long double __sqrtl (long double x) {}

FAKEBUILTIN
long double hypotl (long double x, long double y) {}
long double __hypotl (long double x, long double y) {}

FAKEBUILTIN
long double cbrtl (long double x) {}
long double __cbrtl (long double x) {}

FAKEBUILTIN
long double ceill (long double x) {}
long double __ceill (long double x) {}

FAKEBUILTIN
long double fabsl (long double x) {}
long double __fabsl (long double x) {}

FAKEBUILTIN
long double floorl (long double x) {}
long double __floorl (long double x) {}

FAKEBUILTIN
long double fmodl (long double x, long double y) {}
long double __fmodl (long double x, long double y) {}

int __isinfl (long double value) {}

int __finitel (long double value) {}

int isinfl (long double value) {}

int finitel (long double value) {}

FAKEBUILTIN
long double dreml (long double x, long double y) {}
long double __dreml (long double x, long double y) {}

FAKEBUILTIN
long double significandl (long double x) {}
long double __significandl (long double x) {}

FAKEBUILTIN
long double copysignl (long double x, long double y) {}
long double __copysignl (long double x, long double y) {}

FAKEBUILTIN
long double nanl (const char *tagb) {}
long double __nanl (const char *tagb) {}

int __isnanl (long double value) {}

int isnanl (long double value) {}

FAKEBUILTIN
long double j0l (long double x) {}
long double __j0l (long double x) {}
FAKEBUILTIN
long double j1l (long double x) {}
long double __j1l (long double x) {}
FAKEBUILTIN
long double jnl (int n, long double x) {}
long double __jnl (int n, long double x) {}
FAKEBUILTIN
long double y0l (long double x) {}
long double __y0l (long double x) {}
FAKEBUILTIN
long double y1l (long double x) {}
long double __y1l (long double x) {}
FAKEBUILTIN
long double ynl (int n, long double x) {}
long double __ynl (int n, long double x) {}

FAKEBUILTIN
long double erfl (long double x) {}
long double __erfl (long double x) {}
FAKEBUILTIN
long double erfcl (long double x) {}
long double __erfcl (long double x) {}
FAKEBUILTIN
long double lgammal (long double x) {}
long double __lgammal (long double x) {}

FAKEBUILTIN
long double tgammal (long double x) {}
long double __tgammal (long double x) {}

FAKEBUILTIN
long double gammal (long double x) {}
long double __gammal (long double x) {}

long double lgammal_r (long double f, int *signgamp) {}
long double __lgammal_r (long double f, int *signgamp) {}

FAKEBUILTIN
long double rintl (long double x) {}
long double __rintl (long double x) {}

FAKEBUILTIN
long double nextafterl (long double x, long double y) {}
long double __nextafterl (long double x, long double y) {}

FAKEBUILTIN
long double nexttowardl (long double x, long double y) {}
long double __nexttowardl (long double x, long double y) {}

FAKEBUILTIN
long double remainderl (long double x, long double y) {}
long double __remainderl (long double x, long double y) {}

FAKEBUILTIN
long double scalbnl (long double x, int n) {}
long double __scalbnl (long double x, int n) {}

FAKEBUILTIN
int ilogbl (long double x) {}
int __ilogbl (long double x) {}

FAKEBUILTIN
long double scalblnl (long double x, long int n) {}
long double __scalblnl (long double x, long int n) {}

FAKEBUILTIN
long double nearbyintl (long double x) {}
long double __nearbyintl (long double x) {}

FAKEBUILTIN
long double roundl (long double x) {}
long double __roundl (long double x) {}

FAKEBUILTIN
long double truncl (long double x) {}
long double __truncl (long double x) {}

FAKEBUILTIN
long double remquol (long double x, long double y, int *quo) {}
long double __remquol (long double x, long double y, int *quo) {}

FAKEBUILTIN
long int lrintl (long double x) {}
long int __lrintl (long double x) {}
FAKEBUILTIN
long long int llrintl (long double x) {}
long long int __llrintl (long double x) {}

FAKEBUILTIN
long int lroundl (long double x) {}
long int __lroundl (long double x) {}
FAKEBUILTIN
long long int llroundl (long double x) {}
long long int __llroundl (long double x) {}

FAKEBUILTIN
long double fdiml (long double x, long double y) {}
long double __fdiml (long double x, long double y) {}

FAKEBUILTIN
long double fmaxl (long double x, long double y) {}
long double __fmaxl (long double x, long double y) {}

FAKEBUILTIN
long double fminl (long double x, long double y) {}
long double __fminl (long double x, long double y) {}

int __fpclassifyl (long double value) {}

int __signbitl (long double value) {}

FAKEBUILTIN
long double fmal (long double x, long double y, long double z) {}
long double __fmal (long double x, long double y, long double z) {}

FAKEBUILTIN
long double scalbl (long double x, long double n) {}
long double __scalbl (long double x, long double n) {}

int signgam;

// struct exception
// {
//     int type;
//     char *name;
//     double arg1;
//     double arg2;
//     double retval;
// };

static inline void __taint_exception(struct exception * p)
{/*T:H*/
    __DO_TAINT(p->name);
}

int matherr (struct exception *exc) {
    __taint_exception(exc);
}

// quarl 2006-07-11
//    The following aren't normal glibc functions; they're just here to
//    generate the fake builtins.

FAKEBUILTIN
double huge_val (void) {}

FAKEBUILTIN
float huge_valf (void) {}

FAKEBUILTIN
long double huge_vall (void) {}

FAKEBUILTIN
double inf (void) {}

FAKEBUILTIN
float inff (void) {}

FAKEBUILTIN
long double infl (void) {}

FAKEBUILTIN
double nans (const char *str) {}

FAKEBUILTIN
float nansf (const char *str) {}

FAKEBUILTIN
long double nansl (const char *str) {}
