// simple test of function template overloading

template<class T>
int f(long, T) {
}

template<class T>
int f(int x, T) {
  return x;                     // bad
}

template<class T>
int f(float, T) {
}

int main() {
  int $tainted x;
  int x2 = x;                   // launder $tainted
  int $untainted y;
  // the normal function should win instead of the function template
  y = f(x2, 3);
}
