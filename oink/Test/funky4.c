//  int $_1 f(int $_1 x);
int f(int x) {return x;}
int main() {
  int $tainted a;
  int b;
  b = f(a);
}
