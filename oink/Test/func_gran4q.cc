// for testing the function granularity CFG computation

void f() {}

void g() {
  f();
}

// not called
void notcalled(int x) {
  int $untainted y = x;
}
void notcalled2() {
  int $tainted x;
  notcalled(x);
}

struct Z {
  Z() {
    g();
  }
  Z(int y) {
    notcalled(3);
  }
};

Z z;
