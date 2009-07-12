// show that const and nonconst propagate through user references
// correctly
int
  const                         // bad
  x;
int &g() {return x;}
int main() {
  g()++;
}
