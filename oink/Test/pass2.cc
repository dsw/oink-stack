// parameter passing by value

struct A {};

void f(A a1) {
  A $untainted &a2
    = a1                        // bad
    ;
}

int main() {
  // avoid problems with non-polymorphic cdtor calls by making a
  // pointer
  A $tainted *a;
  f(*a);
}
