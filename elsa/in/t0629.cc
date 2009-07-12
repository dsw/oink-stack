// t0629.cc
// overloaded function, two viable candidates, and an overloaded
// function is among the arguments

// reduced from a testcase reported by Umesh Shankar

void f(int (*)(), int);
void f(int (*)(), char);

int g();
int g(int);

void h()
{
  f(g, 3);
}

// EOF
