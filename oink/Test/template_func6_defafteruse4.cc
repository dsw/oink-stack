// test function template forwarding with specialization declared and
// defined after use; this should not work; Scott says "See cppstd
// 14.7.3 para 6."  It does in g++; we should give a user error so
// that we catch this or else the analysis is unsound

// forward
template<class T> T g(T x);

template<class T> T f(T x) {
  return g(x);
}

template<class T> T g(T x) {
  // no return
}

// NOTE: no declaration of "template<> int g(int x)" here

int main() {
  int $tainted a;
  // launder the $tainted off of the type so function template
  // argument inference doesn't go an infer the return type to be an
  // int $tainted
  int a2 = a;
  int $untainted b;
  b = f(a2);
}

template<> int g(int x) {
  return x;                     // bad
}
