// simple test of function template overloading

template<class T>
int f(T x4) {
}

// stronger than the function template when it comes to overloading
int f(int x3) {
  return x3;                    // bad
}

int main() {
  int $tainted x;
  int x2 = x;                   // launder $tainted
  int $untainted y;
  // the normal function should win instead of the function template
  y = f(x2);
}
