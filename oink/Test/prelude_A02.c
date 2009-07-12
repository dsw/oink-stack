// test funky
void f(char $_1 *a, char $_1_2 *b);

int main() {
  char $tainted *x;
  char $untainted *y;
  f(x, y);
}
