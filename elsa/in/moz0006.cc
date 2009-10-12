// moz0006.cc
// Tests bugs that were affecting jsgc.ii, namely 
//   (a) space for the vptr was not counted in the sizeof computation
//   (b) alignment computations were totally wrong

class A {
  // vptr should take up 4 bytes
  virtual void f();

  // size 1 byte, cur size = 5
  char c;

  // size 4 bytes, aligned at 8, for total size of 12
  int n;
};

int check[sizeof(A) == 12];
