int $_1_2 boink(int a, ... $_2);
int main() {
// x should get tainted
  int x;
  int y;
  int $tainted z;
  x = boink(1, 2, y, z);
}
