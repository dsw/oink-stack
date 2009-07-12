// Assertion failed: illegal atomic type tag, file oink_integrity.cc line 289

template<class T> struct D {
  bool operator == (const D<T> &m) {
    this == &m;
  }
};

D<int> c;
