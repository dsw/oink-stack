// test that throwing an exception gets an edge
int main() {
  try {
    int $tainted y;
    throw y;                    // bad
  } catch (int $untainted x) {
  }
}
