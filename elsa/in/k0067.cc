// constructor using volatile reference

// originally found in package 'bidwatcher'

// Assertion failed: fullyQualifiedName called on scope that doesn't terminate in the global scope, file cc_scope.cc line 1176
// Failure probably related to code near k0067.cc:14:12
// current location stack:
//   k0067.cc:14:12
//   k0067.cc:9:12
//   k0067.cc:9:6

// ERR-MATCH: fullyQualifiedName called on scope that doesn't terminate in the global scope

volatile bool x;

void foo() {
  struct AutoFlag {
    volatile bool &b;
    AutoFlag(volatile bool & b0): b(b0) {}
  };
  AutoFlag a(x);
}
