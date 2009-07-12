// test forwarding of function template primary declared before but
// defined after it is used, but only used in another function
// template

// forward
template<class T> T g(T x);

template<class T> T f(T x) {
  return g(x);
}

template<class T> T g(T x) {
  return x;                     // bad
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
