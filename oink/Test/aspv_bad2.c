struct A {
  int $!argstackparam *y2;      // class
};

void g(int $!argstackparam *y2);

int main() {
  struct A a;
  // a class member variable, but this is hidden by the other problem
  // that '.' is not an E_variable expression
  g(a.y2);
}
