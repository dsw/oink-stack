// TypeFactory_O::applyCVToType must union the cv qualifiers, not
// replace them

struct A {};

typedef const A constA;

void f0 (constA &);

int f1 () {
  f0(A());
}
