// specialization of template function member

// originally found in package 'fluxbox_0.9.11-1sarge0'

// Assertion failed: vfd == this, file cc_tcheck.cc line 694
// Failure probably related to code near a.ii:7:24
// current location stack:
//   a.ii:7:24
//   a.ii:7:1

// ERR-MATCH: Assertion failed: vfd == this

template <typename T> struct S1 {
  void foo() {}
};

template <> void S1<int>::foo() {}
