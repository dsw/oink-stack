// for testing the function granularity CFG computation

void f() {}

void g() {
  f();
}

// not called
void notcalled() {}
void notcalled2() {
  notcalled();
}

struct Z {
  Z() {
    g();
  }
  Z(int y) {
    notcalled();
  }
};

Z z;
