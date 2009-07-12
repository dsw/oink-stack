// ERR-MATCH: a6587633-0ba2-452d-9d78-184adb1be79c|Assertion failed.*getReal.*oink_var.cc

// S1::foo has inline definition, S2::foo does not.

template <typename C>
struct S1 {
  virtual void foo() {
  }
};

template <typename C>
struct S2 : S1<C> {
  virtual void foo();
};

int main() {
  S2<int> s2;
}
