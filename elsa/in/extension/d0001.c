// test annotations

int a;                          // no annotations on top-level decls

int main() {
  int b;                        //* a local var
  while(true) {
    break;                      //* a break statement
  }
  return 0;                     //* a return statement
}
