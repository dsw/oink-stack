// from Taras

struct Foo {
  void *v;
  Foo() : v(0) {}
  Foo(void *v) : v(v) {}
  Foo(Foo const &other) : v(other.v) {}
};

bool deep() {
  return false;
}

int main() {
  Foo f = deep() ? Foo() : 0;
  return 0;
}
