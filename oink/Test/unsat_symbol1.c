// test one-file unsatisfied symbol detection

void foo();

int main() {
  foo();                        // bad
}
