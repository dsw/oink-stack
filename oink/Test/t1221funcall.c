int f(int q) {return q;}
int main() {
  int $tainted x;
  int $untainted y;
  f(x);                         // good
  y = f(x);                     // bad
}
