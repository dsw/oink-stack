// test input for -fx-heapif-stack-arrays

// global array: should not be heapified
int z[3];

int f(int const i, int const j, int *k) {
  return i;
}

int main() {

  // non-array: should not be heapified
  int a;

  // simple array: should be heapified
  int b[10];

  int i;
  for (i=0; i<10; ++i) {
    b[i] = 10 * i;
  }

  f(b[3], 3, &b[2]);

  return 0;
}
