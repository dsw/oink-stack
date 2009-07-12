// Another version of virtual4.cc that used to fail as well.

template <typename C>
struct S1 {
  int x;
  virtual void foo() {}
};

template <typename C>
struct S2 : public S1<C> {
  int y;
  virtual void foo() {}
};

int main() {
  S2<int> s;
  s.foo();
}
