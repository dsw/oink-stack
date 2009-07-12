//  * = func
typedef int (func1_t)(int);
//  int y(int q) {return q;}
int main() {
  int * $tainted x;             // bad
  int $tainted * x;             // good
  func1_t $tainted z;
  x = z;                        // bad
}
