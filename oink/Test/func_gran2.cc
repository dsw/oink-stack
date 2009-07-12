// for testing the function granularity CFG computation

struct C {
  void c0() {}
  void notcalled() {}
};

int main() {
  void (C::*cptm)() = &C::c0;
}
