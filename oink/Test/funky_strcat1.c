char $_1_2 *strcat(char $_1_2 *dest, const char $_1 *src);

int main() {
  char $tainted *x;
  char $untainted *y;
  strcat(y, x);
}
