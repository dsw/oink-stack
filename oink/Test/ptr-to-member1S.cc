// pointer to static function member is just a normal pointer
class A {
  public:
  static                        // NOTE!
    int f(int x) {}
};

int main() {
  int (A::*quint)(int);
  quint = &A::f;                // fails
}
