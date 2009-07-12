// test class templates; simple instantiation

template<int I, class T>
struct A {
  T y;
  A(T y0) : y(y0) {}
  T getY() {return y;}
};

//  struct A0 {
//    int y;
//    A0(int y0) : y(y0) {}
//    int getY() {return y;}
//  };

int $tainted aIn;

A<1, int> a(aIn);
//  A0 a(aIn);

int main() {
  int $untainted aOut  = a.getY(); // bad
}
