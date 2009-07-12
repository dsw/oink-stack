// compile-time type size checking - long long

// originally found in package 'ucl'

// k0010a.cc:5:12: error: array size must be nonnegative (it is -1)
// k0010a.cc:6:12: error: array size must be nonnegative (it is -1)
// k0010a.cc:7:12: error: array size must be nonnegative (it is -1)
// k0010a.cc:8:12: error: array size must be nonnegative (it is -1)
// k0010a.cc:9:12: error: array size must be nonnegative (it is -1)

// ERR-MATCH: array size must be nonnegative

extern int x[1-2*!(9223372036854775807LL > 0)];
extern int x[1-2*!(-9223372036854775807LL - 1 < 0)];
extern int x[1-2*!(9223372036854775807LL % 2147483629l == 721)];
extern int x[1-2*!(9223372036854775807LL % 2147483647l == 1)];
extern int x[1-2*!(9223372036854775807ULL % 2147483629ul == 721)];
