// works
int main() {
  int $tainted **x;
//    void **y;
  void *y;
  int **z;
  y = x;                        // fails
  z = y;
}
