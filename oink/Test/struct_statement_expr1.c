// ERR-MATCH: 39ce4334-0ca1-4e19-aaf9-7f27f335a629

struct S1 { int foo; char bar; };

struct S1 func () {
  struct S1 v;

  ({ return v; });
}

int main() {}
