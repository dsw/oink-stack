// for testing the function granularity CFG computation

void functionptr_notcalled() {
  int $tainted x;
  int $untainted y;
  y = x;                        // should never show up
}
void functionptr1() {}
void functionptr2() {}

int main() {
  void (*q)() = functionptr1;
  void (*q2)() = &functionptr2;
}
