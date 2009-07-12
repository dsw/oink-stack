static int gronk(int x);        // bad: static delared but not defined
int main() {
  int x;
  int y;
  y = gronk(x);
}
static int foo(int x);          // bad: static delared but not defined
