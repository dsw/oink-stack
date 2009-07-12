// ERR-MATCH: 1f04795f-4345-47a0-b7f8-dd8bb1ee4438

// qual: Assertion failed: !externVisitedSet.contains(qv) && "1f04795f-4345-47a0-b7f8-dd8bb1ee4438", file qual_annot.cc line 481

typedef void (*func_takes_int) (int);

void bar (func_takes_int f) {}

void foo () {}

int main() {
  bar(foo);
}
