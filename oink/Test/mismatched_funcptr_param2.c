// ERR-MATCH: d61f1ed1-a219-4e5b-b6cb-7c11bea92d73

// qual: Assertion failed: LibQual::extern_qual(annot->qv()) &&
// "d61f1ed1-a219-4e5b-b6cb-7c11bea92d73", file qual.cc line 76

typedef void (*func_takes_int) (int);

void bar (func_takes_int f) {}

static void foo () {}

int main() {
  bar(foo);
}
