// test that throwing an exception gets an edge
class A {
  public:
};

int main() {
  try {
    A $tainted *a0;
    throw a0;                   // bad
  } catch (... $untainted) {
  }
}
