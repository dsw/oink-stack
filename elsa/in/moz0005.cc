// moz0005.cc
// Non-moz Elsa can parse this file just fine, but it didn't instantiate
// the template because it was in an array. This test is meant to document
// the bug. The desired behavior is to have the template instantiated. This
// can be verified with ccparse -tr printTypedAST.

template<typename T>
class A {
};

class B {
  A<int> a[10];
};
