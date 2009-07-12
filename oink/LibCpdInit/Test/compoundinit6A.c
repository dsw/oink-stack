// compoundinit6A.c; see License.txt for copyright and terms of use

// test range designator initializers
int main() {
  int x[8] = { [1 ... 3] = 1, [5 ... 7] = 2};
  int y[8] = { [1 ... 3] = 1, 13, [5 ... 7] = 2};
  int z[8] = { [1 ... 3] = 1, [5 ... 7] = 2};
}
