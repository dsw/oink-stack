// t0614.cc
// explicitly specialize a member that has an inline defn in primary

// previously caused an assertion failure "vfd == this" in
// cc_tcheck.cc, at the end of Function::tcheckBody()

// Reported by Umesh Shankar.

template <class T>
class A {
public:
  // problem only happens with an inline definition
  void foo() {}
};

template <>
void A<int>::foo()
{}

// EOF
