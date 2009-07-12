// test class templates; partial specialization / type constant

template<int I, class T>
struct A {
  T y;
  A(T y0) : y(y0) {}
  T getY() {/*return y;*/}
};

// partial specialization
template<class T>
struct A<2, T> {
  T y;
  A(T y0) : y(y0) {}
  T getY() {return y;}
};

int $tainted a1In;
A<
  // the good case picks the primary, and the bad case picks the
  // specialization
  1                             // good
  2                             // bad
  , int> a1(a1In);

int main() {
  int $untainted aOut = a1.getY();
}
