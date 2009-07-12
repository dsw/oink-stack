// test class templates; partial specialization / pointer-to-member type constant

struct B {};

template<class T>
struct A {
  T y;
  A(T y0) : y(y0) {}
  T getY() {/*return y;*/}
};

// partial specialization
template<class T, class S>
struct A<S T::*>
{
  T x;
  S y;
  A(S y0) : y(y0) {}
  S getY() {return y;}
};

int $tainted a1In;
A<
  // the good case picks the primary, and the bad case picks the
  // specialization
  int                           // good
  int B::*                      // bad
  > a1(a1In);

int main() {
  int $untainted aOut = a1.getY();
}
