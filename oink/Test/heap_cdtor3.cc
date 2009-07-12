// heap dtors

struct A {
  ~A() {
    A $untainted *a = this;
  }
};

int main() {
  A $tainted *a = new A();
  delete a;                     // bad
}
