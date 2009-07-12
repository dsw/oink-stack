// works
extern int z;
int z = (int $tainted) 4;

// works
extern int $tainted w2;
extern int w2;
int w2;

// doesn't work ****************
extern int w;
extern int $tainted w;
int w;

// doesn't work ****************
extern int w3;
int $tainted w3;

int main() {
  int $tainted y;

  // works
  {
    int x;
    x = y;
  }

  // works!
  {
    int x = y;
  }
}

// works
int foo(int $tainted x) {
  return x;
}

// doesn't work
int foo2(int $tainted x);
int foo2(int $tainted x) {
  return x;
}

// doesn't work
int foo3(int x);
int foo3(int $tainted x) {
  return x;
}

// doesn't work
int foo4(int $tainted x);
int foo4(int x) {
  return x;
}

// works
int gronk(int x) {
  return x;
}

// doesn't work
int gronk2(int x);
int gronk2(int x) {
  return x;
}

void oink() {
  int $tainted y;
  // works
  {int x = foo(1);}
  {int x = foo2(1);}
  {int x = foo3(1);}
  {int x = foo4(1);}

  {int x = gronk(y);}
  {int x = gronk2(y);}
}
