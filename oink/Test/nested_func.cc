// nested functions
int main() {
  int f() {
    int x = f();
    return x;
  }
  int y;
  y = f();
}
