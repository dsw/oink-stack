// test funky
int $_1_2 f(int $_1 q);
int main() {
  int $tainted x;
  int $untainted y;
  y = f(x);
}
