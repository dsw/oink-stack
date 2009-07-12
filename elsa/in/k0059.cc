// implicit type conversion in operator delete

// originally found in qt-x11-free

// k0059.cc:15:3: error: can only delete pointers, not `struct QGuardedPtr'
// typechecking results:
//   errors:   1
//   warnings: 0

// ERR-MATCH: can only delete pointers, not `.*'

struct QGuardedPtr
{
  operator int*() const {}
};

int main()
{
  QGuardedPtr p;
  delete p;
}


// too many conversion operators
struct A {
  operator int* ();
  //ERROR(1): operator float* ();
};

void f()
{
  A a;
  delete a;
}


// too few conversion operators{
struct B {
};

void g()
{
  B b;
  //ERROR(2): delete b;
}



