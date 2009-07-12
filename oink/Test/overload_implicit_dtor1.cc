int main() {
  struct fooB {int xB;};
  struct fooB *fB = new fooB();
  fB->~fooB();
}
