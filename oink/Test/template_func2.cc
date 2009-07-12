// test template function complete specialization

template<int I, class T> T f(T x) {
  //  return x;
}

template<> int f<2, int>(int x) {
  return x;
}

int main() {
  int $tainted y;
  int $untainted z =
    f<
      1                         // good
      2                         // bad
      , int>
    (y);
}
