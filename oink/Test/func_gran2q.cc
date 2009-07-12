// for testing the function granularity CFG computation

struct C {
  void c0() {}
  void notcalled() {
    int $tainted x;
    int $untainted y;
    y = x;                      // should never show up
  }
};

int main() {
  void (C::*cptm)() = &C::c0;
}
