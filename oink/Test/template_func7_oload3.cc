// simple test of function template overloading

// stronger than the function template when it comes to overloading
int f(int x3) {
  return x3;                    // bad
}

template<class T>
int f(T) {
}

int main() {
  int $tainted x;
  int x2 = x;                   // launder $tainted
  int $untainted y;
  // the normal function should win instead of the function template
  y = f(x2);
}
