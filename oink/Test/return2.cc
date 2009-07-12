// return by value to assignment

struct A {};

A f() {
  A $tainted *a1;
  A *a2;
  // NOTE: we avoid the non-polymorphic dtor call collision with a
  // below by making a pointer and derefing it.
  return *a1;                   // bad
  return *a2;                   // good
}

int main() {
  A $untainted a;
  a = f();
}
