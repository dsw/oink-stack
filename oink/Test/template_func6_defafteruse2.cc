// test function template forwarding of primary with declaration
// before but definition after use

// forward
template<class T> T g(T x);

template<class T> T f(T x) {
  return g(x);
}

int main() {
  int $tainted a;
  // launder the $tainted off of the type so function template
  // argument inference doesn't go an infer the return type to be an
  // int $tainted
  int a2 = a;
  int $untainted b;
  b = f(a2);
}

template<class T> T g(T x) {
  return x;                     // bad
}
