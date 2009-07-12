// atomic = array (possible?)
int main() {
  int $tainted x;
  int $tainted y [];
  x = y;                        // bad
}
