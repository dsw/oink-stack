// show that const and nonconst propagate through user references
// correctly
void g(int
       // NOTE: the whole point of this test is that you can omit
       // 'const' here and our const inference will find the problem
       // anyway.  However, recent (24 Sept 2004) changes in elsa have
       // strengthened the typechecking to the point where this is no
       // longer legal; thus, we need to no run this test until there
       // is a flag that allows const to be ignore in elsa
       &                        // bad
       x) {x++;}
int main() {
  int const y;
  g(y);
}
