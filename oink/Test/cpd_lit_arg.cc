// Pass a compound literal as an argument
struct A {
  int x;
};

void f(A a) {
}

int main() {
  f((A) {3});
  f(((A) {3}));
}
