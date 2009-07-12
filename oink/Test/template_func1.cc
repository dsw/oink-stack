// test standard template function

template<class T> T f(T x) {
  return x;                     // bad
}

int main() {
  int $tainted y;
  int $untainted z = f<int>(y);
}
