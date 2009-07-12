void g(int $!argorderef **s);

void f(int, int $!argorderef **s, char *) {
  *s;
  g(s);
}

int main() {
  int $!argorderef x;
  x + 1;                        // ok because not **
  int $!argorderef *y;
  y + 1;                        // ok because not **
  int $!argorderef ***z;
  z + 1;                        // ok because not **
  int $!argorderef **q;
  f(3, q, "hello");             // ok because in a function argument
  *q;                           // ok because in a deref
}
