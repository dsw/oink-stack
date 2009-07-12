// templatized template parameter list

// ERR-MATCH: 25eb7000-851c-4028-b762-4e365a5b10bf

// from boost, originally seen in package 'monotone'

// Assertion failed: unimplemented: templatized template parameter list
// (25eb7000-851c-4028-b762-4e365a5b10bf), file cc.gr line 2395

template< template< typename T1, typename T2, typename T3 > class F, typename Tag >
struct quote3;
