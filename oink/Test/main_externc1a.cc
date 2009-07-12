// 'main' is declared as a C function in main_externc1b.c; defined in
// main_externc1a.cc without 'extern "C"'

int printf(char $untainted * fmt, ...) {}

int main(int argc, char **argv)
{
  printf(argv[0]);                                  // bad
}
