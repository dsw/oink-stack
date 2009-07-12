// nested template class

// originally found in package 'monotone' (Boost headers)

// In state 159, I expected one of these tokens:
//   <name>,
// k0066.cc:12:19: Parse error (state 159) at <

// ERR-MATCH: Parse error .state (157|159). at <

template< template< typename T1 > class F >
struct S;
