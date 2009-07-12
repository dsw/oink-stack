// friend class as template parameter

// originally found in package 'zipios'

// typechecking results:
//   errors:   0
//   warnings: 0
// error: k0069.cc:15:12: internal error: found dependent type `Friend' in non-template

// ERR-MATCH: internal error: found dependent type `([^(]|[(][^d]|[(][d][^e]).*?' in non-template

template< class Type > struct T {
  friend struct Friend ;
};

struct Friend;

typedef T< Friend > T1 ;
