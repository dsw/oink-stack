// t0579.cc
// enumerators WTF?  7.2p4(!),5

void f()
{
  // everybody thinks 'E' is visible, but gcc rejects
  // b/c E is incomplete (underlying type not determined)
  //enum E { e1 = sizeof(E) };
}


template <int n>
struct A {    typedef int NONE; };

template <>
struct A<1> { typedef int ONE; };

template <>
struct A<4> { typedef int FOUR; };


void g()
{
  enum E {
    e1 = 'a',                     // type char
    e2 = 1,                       // type int
    e3 = (unsigned char)0xff,     // type unsigned char
    e4,                           // type is unspecified integral type

    e1s = sizeof(e1),             // 1
    e2s = sizeof(e2),             // 4
    e3s = sizeof(e3),             // 1
    e4s = sizeof(e4),             // ? EDG goes to 8!
    
    one = 1
  };

  // confirm specializations are working right
  A<one>::ONE a0;

  A<e1s>::ONE a1;
  A<e2s>::FOUR a2;
  A<e3s>::ONE a3;
  //A<e4s>::FOUR a4;

}



