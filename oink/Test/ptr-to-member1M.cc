// if you take the address within parens you get a normal pointer
class A {
  public:
  int f(int x) {}
};

int main() {
  int (A::*quint)(int);
  // This makes a normal pointer.
  quint = &(A::f);
}
