struct A {
  void f(int x){}
  void g() {
    (f)(3);                     // parens are allowed around method name
  }
};

int main() {
  A *a;
  a->g();
}
