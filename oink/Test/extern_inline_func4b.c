int myprintf(char $untainted * fmt, ...) {}

char foo();

int main() {
  char c = foo();
  myprintf(&c);
}
