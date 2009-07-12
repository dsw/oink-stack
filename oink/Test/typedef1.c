// test that qualifiers on typedefs are preserved

int main() {
  typedef int $tainted myint;
  myint x;
  int $untainted y;
  y = x;
}
