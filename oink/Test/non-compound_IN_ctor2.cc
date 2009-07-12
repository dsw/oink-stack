// test MemberInit correctly initializes a non-compound type, here an
// int, within an IN_ctor

int $tainted i0;

struct A {
  int i1;
  A()
    : i1(i0)                    // an IN_ctor for a non-compound type
  {}
};

int main() {
  A a;
  int $untainted u1;
  u1 = a.i1;
  return 0;
}
