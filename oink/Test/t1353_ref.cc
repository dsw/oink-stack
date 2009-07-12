// show that const and nonconst propagate through user references
// correctly
int main() {
  int
    const                       // bad
    x;
  int y;
  bool a;
  (a?x:y)++;
}
