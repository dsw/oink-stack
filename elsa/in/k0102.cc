// template parameter template-instantiated on another template paramter

// ERR-MATCH: 64103c40-efae-4068-b4b1-5492a549b00c

// from boost

// Assertion failed: ret.hasValue() && "64103c40-efae-4068-b4b1-5492a549b00c",
// file template.cc line 4038

template< int N > struct A {
  typedef int fast;
};

template < int N, typename A<N>::fast foo > struct S {
};

int main() {
  S<1, 2> s;
}
