// function template argument deduction from function arguments

template<class T>
T max1(T x, T y) {
//    return x;
}

// complete specialization
template<>
// NOTE: this will not work, since the reference-ness goes away
// during unification:
//  int &max1<int&>(int &x, int &y) {
// use this instead:
int max1<int>(int x, int y) {
  return x;                     // bad
}

int main() {
  int $tainted x0;
  int x1 = x0;                  // launder the type so $tainted isn't used to instantiate
  int y1;
  // NOTE: the absence of template arguments
  int $untainted z1 = max1(x1, y1);
}
