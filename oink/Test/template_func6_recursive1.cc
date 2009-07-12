// test if recursion works

template<class T> T f(T x, T y) {
  if (1) {                      // cqual doesn't see this is determinted at compile time
    return x;                   // bad
  } else {
    return f(y, 0);
  }
}

int main() {
  int $tainted a;
  // launder the $tainted off of the type so function template
  // argument inference doesn't go an infer the return type to be an
  // int $tainted
  int a2 = a;
  int $untainted b;
  b = f(3, a2);
}
