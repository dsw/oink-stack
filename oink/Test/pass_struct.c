struct S {
    int a;
};

void foo (struct S s) {
    int $untainted i = s.a;
}

int main() {
  struct S s;
  int $tainted t;
  s.a = t;
  foo(s);                       // bad
}
