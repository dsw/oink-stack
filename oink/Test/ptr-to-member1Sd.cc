// pointer to static data member is just a normal pointer
class A {
  public:
  static                        // NOTE!
    int x;
};

int main() {
  int A::*quint;
  quint = &A::x;                // fails
}
