struct A {
  virtual int foo(int x) { return 0; }
};

struct B : A {
  int foo(int x) { return x; }
};

int main()
{
  A * a = new B;

  int $tainted t;
  int $untainted u;

  u = a->foo(t);
}
