// test forwarding and mutual recursion for specializations

// forward
template<class T> T g(T x, T y);

// primary
template<class T> T g(T x, T y) {
}

// forward
template<class T> T f(T x, T y);

// primary
template<class T> T f(T x, T y) {
}

// forward the specialization
template<> int g<int>(int x, int y);

// specialization
template<> int f<int>(int x, int y) {
  if (1) {                      // cqual doesn't see this is determined at compile time
    return x;                   // bad
  } else {
    return g(0, y);
  }
}

// specialization
template<> int g<int>(int x, int y) {
  if (1) {                      // cqual doesn't see this is determined at compile time
    return 17;                  // irrelevant
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
  b = f(0, a2);
}
