// test the reference policy

template<class T>
  T const &max1
  // NOTE 1: one is a reference and one is not.  g++ likes it.
  (T &x, T y) {
  return x;                     // bad
}

int main() {
  int $tainted x0;
  int x1 = x0;                  // launder the type
  int y1;

  int $untainted z1 = max1(x1, y1);

  // NOTE 2: the referenceness of the first parameter x above is not
  // for naught.  If you try to pass a '3' (below) it doesn't work.
  // However if you reverse the arguments, you can pass a '3' to y.

//  template_func5_arg_deduce1.cc.filter-good.cc:19: could not convert `3' to `int&'
//  template_func5_arg_deduce1.cc.filter-good.cc:4: in passing argument 1 of `const
//     T& max1(T&, T) [with T = int]'

//    int /*$untainted*/ z1 = max1(3, y1);  // reverse the arguments and it is ok.
}
