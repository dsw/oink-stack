//  func = array (possible?)
typedef int (func1_t)(int);
int main() {
  funct1_t $untainted x;
  int z [] $tainted;
  x = z;                        // bad
}
