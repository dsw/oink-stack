// test that throwing an exception gets an edge
// for ellipsis exceptions
int main() {
  try {
    int $tainted y;
    throw y;                    // bad
  } catch (... $untainted) {
  }
}
