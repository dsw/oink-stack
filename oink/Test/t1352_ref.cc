// show that const and nonconst propagate through user references
// correctly
int main() {
  struct S {
    int
      const                         // bad
      x;
  };
  S s0;
  s0.x++;
}
