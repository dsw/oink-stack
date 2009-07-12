// member init dtors

struct A {
  A () {
    // make "this" tainted
    A $tainted *a2 = this;
  }
  ~A() {
    A $untainted *a = this;     // bad
  }
};

struct B : A {
  B() : A () {}
};

int main() {
  B b;
}
