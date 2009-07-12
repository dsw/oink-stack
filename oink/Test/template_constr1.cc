// ERR-MATCH: c5826071-1097-43c5-9388-e65f6d852db2

// template class constructor, indirectly instantiated through another
// template's typedef

// oink: a.ii:4: Non-compound type must have one-arg MemberInit
// (c5826071-1097-43c5-9388-e65f6d852db2) a.ii.50

template <typename T1>
struct S1 {
  S1() : i1() {
  }
  int i1;
  typedef int myint;
};

template <typename T2>
struct S2 {
  typedef S1<int> myint;
};

int foo() {
  S2<int>::myint x;
}
