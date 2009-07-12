// should allow $!nonglobal qualified globals
int x;

// should allow $!nonglobal qualified function parameters
int f(int $!nonglobal x, int $!nonglobal *y) {
  return 0;
}

// should allow $!nonglobal qualified stack variables
void g() {
  int $!nonglobal x;
  int $!nonglobal * y;
}
