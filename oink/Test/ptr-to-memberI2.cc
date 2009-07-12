// illegal pointer to member declarations
class A;

int main() {
  int A::*q;
  void A::*r;                   // illegal
//    void const A::*r;
//    void const volatile A::*r;
}
