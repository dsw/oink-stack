// member init dtors

struct A {
  A();
  ~A() {
    A $untainted *a = this;
  }
};

struct B {
  A $tainted a;                 // bad
  B() {}
};

int main() {
  B b;
}
