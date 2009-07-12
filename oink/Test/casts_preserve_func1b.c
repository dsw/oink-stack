// Test that the parameters flow into one another.

int gronk(int $untainted *) {
}

int foo(int $tainted *) {
}

int main() {
  int (*fptr)(int *) = gronk;
  fptr = (int (*)(int *)) foo;
}
