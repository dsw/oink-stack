// #include <stdio.h>
void printf(char *);

struct A {};
struct B : virtual A {};
struct C : virtual protected A {};
struct D : B, private C {};
struct E : A {
  struct F {};
};
struct G : E::F {};

int print_hello(int x) {
  printf("hi");
  int q;
  if (x%2 == 0) {
    q = x/2;
  } else {
    q = x * 3 + 1;
  }
  return q;
}

void print_goodbye(int i) {
  while(true) {
    --i;
    if (i) continue;
    else printf("bye");
    break;
  }
}

int main() {
  int x = 1;
  int y = (int) x;
  int q = print_hello(y);
  print_goodbye(q);
  return 0;
}
