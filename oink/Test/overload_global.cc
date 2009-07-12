// test overloading of globals
int f() {
  int x;
  return x;
}

int f(int) {
  int $tainted y;
  return y;
}

int main() {
  int $untainted z;
  int arg = 3;
  z = f(arg);
}
