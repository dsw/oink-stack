// test function template overloading

template <class T> struct A {};

A<int>& f(A<int>&z) {
  return z;
}

// make sure correct one is in the middle so that we know it won't be
// called by default by being at the top of the overload set

int **f(int **z) {
  return z;                     // bad
}

A<float>& f(A<float>&z) {
  return z;
}

int main() {
  int $tainted **x;
  int **x2 = x;                 // launder taintedness
  int $untainted **y;
  y = f(x2);
}
