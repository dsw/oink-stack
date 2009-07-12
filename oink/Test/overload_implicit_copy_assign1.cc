// test overloading
struct A {
  A() {}
  A(int x) {
    int z;
    z = x;
  }
  // this is a distractor from the implicitly-declared copy assignment
  // operator
  A &operator =(int) { return *this; }
};

int main() {
  int arg = 3;
  A a;
  a = A(arg);
}
