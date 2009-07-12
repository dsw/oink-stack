// show that const and nonconst propagate through user references
// correctly
int main() {
  int
    const                         // bad
    x[10];
  x[3]++;
}
