// stack dtors

struct A {
  ~A() {
    A $untainted *a;
    a = this;                   // bad
  }
};

int main() {
  A $tainted a;
}
