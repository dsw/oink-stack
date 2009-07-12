//  func = func
typedef int (funct1_t)(int);
int main() {
  funct1_t $untainted *x;
  funct1_t $tainted *z;
  x = z;                        // bad
}
