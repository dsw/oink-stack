// Test that the linker doesn't require definitions for pure virtual functions

class C {
  virtual void foo() = 0;
};

class D : C {
  virtual void foo() {}
};

int main()
{
  D d;
  C &c = d;
  c.foo();
  d.foo();
}
