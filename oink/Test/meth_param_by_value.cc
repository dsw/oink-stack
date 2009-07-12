// this one was failing the const inference because the pass by value
// elaboration on the arugments was being messed up by an off by one
// error in the case of a method: the 'this' param was not being
// skipped.

struct B {};

const B *g() {}

struct D {
  void f(const B b, int &a);
};

void D::f(const B b, int &a)
{
  a = 0;
  const B *r = g();
  f(*r, a);
}
