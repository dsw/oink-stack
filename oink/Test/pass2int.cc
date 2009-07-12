// parameter passing by value

struct A {};

void f(int a) {
  int $untainted a2
    = a                         // bad
    ;
}

int main() {
  int $tainted a;
  f(a);
}
