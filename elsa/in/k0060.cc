// disambiguated from multiple base class subobjects

// originally found in package kdelibs

// k0060.cc:21:5: error: field `C1::x' ambiguously refers to elements of multiple base class subobjects
// typechecking results:
//   errors:   1
//   warnings: 0

// ERR-MATCH: error: field `.*' ambiguously refers to elements of multiple base class subobjects

struct B {
  int x;
};

struct C1: B {
};
struct C2: B {
};

struct T : C1, C2 {
};

int main() {
  T t;
  t.C1::x = 0;
}
