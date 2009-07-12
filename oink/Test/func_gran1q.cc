// for testing the function granularity CFG computation

int x;

void f() {}

void g() {
  f();
}

void i() {}

// not called
void notcalled(int x) {
  int $untainted y = x;
}
void notcalled2() {
  int $tainted x;
  notcalled(x);
}

struct A {
  A() {
    g();
  }
  A(int y) {
    notcalled(3);
  }
};

int main() {
  // normal function call
  (((i)))();
  // E_constructor
  A a2(A());
}
