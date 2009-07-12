int main() {
  int $tainted x;
  int *y;
  int $untainted z;
  *y = x;
  z = *y;
//    *y = z;
}
