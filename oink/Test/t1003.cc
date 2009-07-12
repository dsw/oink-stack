// atomic = func (possible?)
typedef int (func1_t)(int);
//  int y(int q) {return q;}
int main() {
  int $tainted x;
  func1_t $tainted z;
  x = z;                        // bad
}
