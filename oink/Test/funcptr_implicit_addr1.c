// ERR-MATCH: 61010461-dc93-4312-bca7-219392176c22

// qual: Assertion failed:
// reinterpret_cast<SObjList<void>*>(target)->isEmpty() &&
// "61010461-dc93-4312-bca7-219392176c22", file ../elsa/xml_reader.cc line 824

int foo1();
int foo2();

void main() {
  int (*foo)();
  foo = foo1;
  foo = foo2;
}
