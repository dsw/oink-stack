// atomic = array (possible?)
int main() {
  int $tainted x;
  int y [] $tainted;
  x = y;                        // bad
}
