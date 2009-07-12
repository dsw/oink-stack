// test overloading of template function and specialization

template<int I, class T> T f(T x) {
  //  return x;
}

// should end up in the same overload set as the first above
template<int I, class T> T f(T x, T x2) {
  return x;
}

//  // NOTE: specialize the SECOND primary
//  template<> int f<2, int>(int x, int y) {
//    return x;
//  }

int main() {
  int $tainted y;
  int y2;
  int $untainted z = f<1, int> (y, y2);
}
