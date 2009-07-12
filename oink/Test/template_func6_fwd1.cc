// test forwarding of function template specializations

// forward primary
template<class T> T g(T x);

// forward specialization
template<> int g<int>(int x);

template<class T> T g(T x) {
  // no return here
}

// this isn't used
template<> float g<float>(float x) {
  // no return here
}

// define specialization
template<> int g<int>(int x) {
  return x;                     // bad
}

int main() {
  int $tainted a;
  // launder the $tainted off of the type so function template
  // argument inference doesn't go an infer the return type to be an
  // int $tainted
  int a2 = a;
  int $untainted b;
  b = g(a2);
}
