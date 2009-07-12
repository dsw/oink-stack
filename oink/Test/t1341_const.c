// test const inference
int main() {
  int a;
  int
    *
    const                       // bad
    b = 0;
  b = &a;
}
