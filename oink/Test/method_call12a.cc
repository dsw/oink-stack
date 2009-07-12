// more tests of the linker imitator; This is file 2.

// this would come from the common header file
struct A {
  virtual void f(int x);
};

// you could pass in a B* here for the A*, but this translation unit
// won't know about that possibility unless the linker imitator tells
// it
void g(A *a)
{
  int $tainted z;
  a->f(z);                      // could call B::f()
}
