// test that we can mark a variable as const when it is a parameter
// only

int $!const_when_param y = 4;

int main() {
  int $!const_when_param x = 3;
  x = 4;                        // should be ok
  y = 5;
  return 0;
}
