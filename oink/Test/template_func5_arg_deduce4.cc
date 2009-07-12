// function template argument deduction from function arguments;
// partial specification at the call site.

template<class T, class S>
S max1(T x, S y) {
//    return x;
}

// complete specialization
template<>
// NOTE: this will not work, since the reference-ness goes away
// during unification:
//  int &max1<int&>(int &x, int &y) {
// use this instead:
short max1<int, short>(int x, short y) {
  return x;                     // bad
}

int main() {
  int $tainted x0;
  int x1 = x0;                  // launder the type so $tainted isn't used to instantiate
  short y1;
  // NOTE: the absence of template arguments
  int $untainted z1 = max1<int>(x1, y1);
}
