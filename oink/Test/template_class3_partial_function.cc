// test class templates; partial specialization / function type constant

template<class T>
struct A {
  T y;
  A(T y0) : y(y0) {}
  T getY() {/*return y;*/}
};

// partial specialization
template<class T, class S, class U>
struct A<U (*)(T, S)> {
  T y;
  A(U y0) : y(y0) {}
  S getY() {return y;}
};

int $tainted a1In;
A<
  // the good case picks the primary, and the bad case picks the
  // specialization
  int                           // good
  int (*)(int, int)             // bad
  > a1(a1In);

int main() {
  int $untainted aOut = a1.getY();
}
