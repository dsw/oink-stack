int main() {
  struct fooB {int xB;};
  struct fooB fB;
  // NOTE: unlike for the default copy assignment operator, we don't
  // need a distractor to test that adding the variable in the
  // presence of overloading works, since the default (no-arg) ctor is
  // providing the distraction
  struct fooB fB2(fB);
}
