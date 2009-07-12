//  func = atomic (int (*)(int) = void * ?)
typedef int (func1_t)(int);
int main() {
  funct1_t $untainted x;
  int $tainted z;
  x = z;                        // bad
}
