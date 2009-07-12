// test template function partial specialization

template<int I, class T> T f(T x) {
  //  return x;
}

template<class S> S f<2, S>(S x) {
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
