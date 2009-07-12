struct A {
  virtual int f();
};

struct B: A {
  virtual int f();
};

int main() {}

/*NOTES

Possible solution: in DataFlowVisitor::visitDeclarator(), skip it if it
doesn't have a funcdefn

make sure to test that data still flows even when the defn is in a separate
translation unit


*/
