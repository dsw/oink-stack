// illegal pointer to member declarations
class A;

int main() {
  int A::*q;
  int & A::*q2;                 // illegal
}
