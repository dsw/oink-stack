// in C, inline functions are not automatically static.

int foo();

int main() {
  return foo();
}
