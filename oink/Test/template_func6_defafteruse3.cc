// test function template forwarding with specialization declared
// before use but defined after use

// forward
template<class T> T g(T x);

template<> int g<int>(int x);

template<class T> T f(T x) {
  return g(x);
}

template<class T> T g(T x) {
  // no return
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

template<> int g<int>(int x) {
  return x;                     // bad
}
