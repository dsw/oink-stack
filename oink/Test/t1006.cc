//  * = array (int * = int[])
int main() {
  int $untainted *x;
  int $tainted y[2];
  x = y;                        // bad
}
