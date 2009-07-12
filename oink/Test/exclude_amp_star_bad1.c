// exclude '&*'
int main() {
  int *x;
  *x = 3;
  int *y = &*x;
  *y = 4;
}
