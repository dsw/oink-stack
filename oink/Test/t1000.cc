// atomic = atomic
int main() {
  int $untainted x;
  int $tainted y;
  x = y;                        // bad
}
