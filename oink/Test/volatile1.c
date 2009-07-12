// this fails to pretty print with volatile attached to the type;
// something not working
int main() {
//    int volatile $tainted &x;
//    x = 3;

  int const volatile x;
  int const volatile & y;

//    int volatile $tainted x;
//    int $nonconst $tainted x;
}
