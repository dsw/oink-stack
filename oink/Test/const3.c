// should fail but doesn't
int main() {
  struct {
    int x;
  } const y = {2};
  y.x = 3;
  return 0;
}
