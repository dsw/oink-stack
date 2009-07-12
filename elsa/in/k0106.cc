// anonymous struct in anonymous union

// ERR-MATCH: there is no member called `.*' in struct

// a.i:12:3: error: there is no member called `a' in struct S
// a.i.13

struct S {
  union {
    struct {
      int a;
    };
  };
};

int main() {
  struct S s;
  int a;
  a = s.a;
}
