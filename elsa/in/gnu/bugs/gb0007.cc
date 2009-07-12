// gb0007.cc
// strange friend + qualified ctor name bug

// rejected by ICC

struct A {
  friend A::A(int);
};
