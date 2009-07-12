// this used to cause an error since one of the failed ambiguous
// branches for the double function call below was creating a variable
// that was ending up in the list of variables for that translation
// unit even thought it had an error type; variables are now not added
// to the list of variables for a translation unit if they have a type
// that contains an error

struct I {};

struct C {};

struct B {
  B(const C &) {}
};

C f(const I &) {}

struct D {
  static I &g() {}
};

int h() {
  B b(f(D::g()));
}
