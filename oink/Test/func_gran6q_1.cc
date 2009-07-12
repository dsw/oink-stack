// for testing the function granularity CFG computation
// test that it works across linking

void f();

// not called
void h(int);

void q() {
  // nothing happening here
}

// not called
void g() {
  int $tainted x;
  h(x);
}

int main() {
  f();
  return 0;
}
