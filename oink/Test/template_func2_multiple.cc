// test template function complete specialization is calling just the
// one we think it is

template<int I, class T> T f(T x) {
  return x;
}

template<> int f<2, int>(int x) {
  return x;
}

template<> int f<3, int>(int x) {
//    return x;
}

template<> int f<4, int>(int x) {
  return x;
}

int main() {
  int $tainted y;
  int $untainted z =
    f<
      2                         // bad
      3                         // good
      , int>
    (y);
}
