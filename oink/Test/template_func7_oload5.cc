// simple test of function template overloading; specialization more
// specific than primary

template<class T>
int f(T) {
}

// g++ just doesn't allow partial specializations of function
// templates.
//  template_func7_oload5.cc:10: error: partial specialization `f<T*>' of function template
//  // specialization
//  template<class T>
//  int f<T*>(T *x) {
//    return *x;                    // bad
//  }

// specialization
template<>
int f<int*>(int *x) {
  return *x;                    // bad
}

int main() {
  int $tainted *x;
  int *x2 = x;                  // launder $tainted
  int $untainted y;
  // specialization should be stronger than primary
  y = f(x2);
}
