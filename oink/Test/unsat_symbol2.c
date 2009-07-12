// test one-file unsatisfied symbol detection

void foo();

void foo() {}                   // good

int main() {
  foo();
}
