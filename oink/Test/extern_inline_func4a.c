extern inline char foo();

char foo() {
  char $tainted c = 'x';
  return c;
}
