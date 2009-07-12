// parameter passing by pointer

struct A {};

void f(A *a) {
  A $untainted *a2
    = a                         // bad
    ;
}

int main() {
  A $tainted *a = new A();
  f(a);
}
