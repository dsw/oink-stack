// 'duplicate definition' from 'using' declaration

// ERR-MATCH: duplicate definition for

// c.ii:9:1: error: duplicate definition for `Foo' of type `double ()(double
// x)'; previous at c.ii:2:8 c.ii.7

double Foo (double x) {
}

namespace N {
  using ::Foo;
}

using N::Foo;
