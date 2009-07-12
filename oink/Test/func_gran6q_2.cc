// for testing the function granularity CFG computation
// test that it works across linking

// not called
void h(int y) {
  int $untainted z;
  z = y;
}

void q();

void f() {
  q();
}
