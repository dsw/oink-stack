// test const inference
int main() {
  int a;
  int
    const                       // bad
    *b = a;
  *b = 3;
}
