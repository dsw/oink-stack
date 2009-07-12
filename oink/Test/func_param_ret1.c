// test that -q-func-param-ret-qual works

// NOTE: the $untainted is attached with the command-line flag
// -q-func-param-ret-qual '$untainted'

void f(int);

int main() {
  int $tainted x;
  int y = x;
  f(y);
}
