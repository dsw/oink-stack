// for testing the function granularity CFG computation

struct B {
  int z;
  B(int z0) : z(z0) {}

  // not called
  virtual void k() {
    int $tainted x;
    int $untainted y;
    y = x;                      // should never show up
  }
};

struct B2 : B {
  // MemberInit
  B2() : B(3) {}
  virtual void k() {}
};

struct B3 : B2 {
  B3() {}
  virtual void k() {}
};

int main() {
  // IN_ctor
  B b(3);
  // E_new
  B2 *b2 = new B3;
  (((b2->k)))();
}
