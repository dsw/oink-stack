// test that throwing an exception gets an edge
class A {
  public:
};

int main() {
  try {
    // we prevent the dtor ~A() from being called by making a pointer
    // and then de-refing it; since ~A() is also called for a1 which
    // is $untainted this prevents a collision in the non-polymorphic
    // analysis
    A $tainted *a0;
    throw *a0;                  // bad
  } catch (A $untainted a1) {   // implicit copy ctor
  }
}
