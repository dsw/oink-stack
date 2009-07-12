//  * = array (int * = int[])
int main() {
  int * $untainted x;
  int y [] $tainted;
  x = y;                        // bad
}
