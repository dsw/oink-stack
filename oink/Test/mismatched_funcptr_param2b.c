// ERR-MATCH: d61f1ed1-a219-4e5b-b6cb-7c11bea92d73

// qual: Assertion failed: LibQual::extern_qual(annot->qv()) &&
// "d61f1ed1-a219-4e5b-b6cb-7c11bea92d73", file qual.cc line 76

typedef void (*MyFunc) (int);

struct S {
  MyFunc func;
};

static void foo();

int main() {
  struct S s = { foo };
}

