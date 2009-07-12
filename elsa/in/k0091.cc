// template specialization with all default parameters

// originally found in package 'kdissert_0.3.8-1'

// Assertion failed: arguments.isNotEmpty(), file template.cc line 396
// Failure probably related to code near a.ii:5:12
// current location stack:
//   a.ii:5:12
//   a.ii:5:12
//   a.ii:5:1

// ERR-MATCH: Assertion failed: arguments.isNotEmpty

template<typename T1 = int> struct S1{};

template<> struct S1<> {};
