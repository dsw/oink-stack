// in C, inline functions are not automatically static.

inline int foo() {}

int main() {
  return foo();
}
