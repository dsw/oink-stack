// should allow noncast qualified globals
int $!noncast x;

// should allow noncast qualified function parameters
int f(int $!noncast x, int $!noncast *y) {
  return 0;
}

// should allow noncast qualified stack variables
void g() {
  int $!noncast x;
  int $!noncast * y;
}

// should allow noncast qualified struct member variables
struct A {
  int $!noncast x;
  int $!noncast * y;
};
