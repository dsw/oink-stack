// test adding an introduction to function calls

int f(int x) {
  return 1;
}

int g() {
  return 0;
}

int main() {
  int x = f(1);
  int y = f(g(x));
  return y;
}
