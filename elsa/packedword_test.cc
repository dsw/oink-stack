// packedword_test.cc            see license.txt for copyright and terms of use

// quarl 2006-06-22
//    test program for PackedWord.

#include "packedword.h"
#include "exc.h"

class Foo {
  PackedWord pw;
  int a, b, c, d;

  PACKEDWORD_DEF_GS(pw, A0, int, 0, 5)
  PACKEDWORD_DEF_GS(pw, B0, int, 5, 20)
  PACKEDWORD_DEF_GS(pw, C0, int, 20, 30)
  PACKEDWORD_DEF_GS(pw, D0, int, 30, 32)

  void check()
  {
    xassert(getA0() == a);
    xassert(getB0() == b);
    xassert(getC0() == c);
    xassert(getD0() == d);
  }

public:
  Foo() : a(0), b(0), c(0), d(0) {}
  void setA(int a0) { setA0(a0); a = a0; check(); }
  void setB(int b0) { setB0(b0); b = b0; check(); }
  void setC(int c0) { setC0(c0); c = c0; check(); }
  void setD(int d0) { setD0(d0); d = d0; check(); }
};

#define N(X) cout << "// " #X ";" << endl; X
#define NE(X)                                                     \
  cout << "// " #X ";" << endl;                                   \
  clog << "Want exception: ";                                     \
  try {                                                           \
    X;                                                            \
    cerr << "OOPS, expected assertion\n";                         \
    return 1;                                                     \
  } catch(...) {                                                  \
    cout << "Good, got assertion.\n";                             \
  }

int main()
{
  N(Foo foo);
  N(int A = (1<<5)-1);
  N(int B = (1<<15)-1);
  N(int C = (1<<10)-1);
  N(int D = (1<<2)-1);
  xassert(A == 31);

  N(foo.setA( 1 ));
  N(foo.setA( A ));

  N(foo.setB( 0 ));
  N(foo.setB( B ));
  N(foo.setA( 0 ));
  N(foo.setC( 0 ));
  N(foo.setD( 0 ));
  N(foo.setC( C ));
  N(foo.setD( D ));
  N(foo.setC( 0 ));
  N(foo.setD( 0 ));

  NE(foo.setA( A+1 ));
  NE(foo.setB( B+1 ));
  NE(foo.setC( C+1 ));
  NE(foo.setD( D+1 ));

  cout << "\n"
       << "packedword_test: PASS.\n"
       << flush;
}

// Local Variables:
// kc-compile-command-make: "make packedword_test && ./packedword_test"
// End:

