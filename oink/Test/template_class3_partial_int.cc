// test class templates; partial specialization / int constant

template<int I, class T>
struct A {
  T y;
  A(int y0) : y(y0) {}
  int getY() {/*return y;*/}
};

// partial specialization
template<int I>
struct A<I, int> {
  int y;
  A(int y0) : y(y0) {}
  int getY() {return y;}
};

int $tainted a1In;
A<1,
  // the good case picks the primary, and the bad case picks the
  // specialization
  int *                         // good
  int                           // bad
  > a1(a1In);

int main() {
  int $untainted aOut = a1.getY();
}
