// SOCKADDR_ARG etc use __attribute__ ((__transparent_union__));

struct S {};

typedef union {
  struct S * x;
  int y;
} U __attribute__ ((__transparent_union__));

int foo (U u) {
  int $untainted z = u.y;
}

int main() {
  int $tainted x;
  foo(x);                       // bad
}
