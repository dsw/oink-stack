// test that -q-func-param-ret-qual works

// NOTE: the $untainted is attached with the command-line flag
// -q-func-param-ret-qual '$untainted'

int main() {
  int $tainted x;
  return x;
}
