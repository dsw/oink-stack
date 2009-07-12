// using incomplete class as default template argument

// originally found in package 'monotone' (Boost headers)

// k0065.cc:13:31: error: attempt to create an object of incomplete class `Foo_'
// typechecking results:
//   errors:   1
//   warnings: 0

// ERR-MATCH: error: attempt to create an object of incomplete class

struct Foo_;
template< typename Tag = Foo_ > struct T;
