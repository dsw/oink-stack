// teset that C++ symbols are reported as fully qualified

namespace A {
  void foo();
};

int main() {
  A::foo();
}
