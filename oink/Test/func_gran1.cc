// for testing the function granularity CFG computation

int x;

void f() {}

void g() {
  f();
}

void i() {}

// not called
void notcalled() {}
void notcalled2() {
  notcalled();
}

struct A {
  A() {
    g();
  }
  A(int y) {
    notcalled();
  }
};

int main() {
  // normal function call
  (((i)))();
  // E_constructor
  A a2 = A();
}
