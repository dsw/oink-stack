// for testing the function granularity CFG computation

void functionptr_notcalled() {}
void functionptr1() {}
void functionptr2() {}

int main() {
  void (*q)() = functionptr1;
  void (*q2)() = &functionptr2;
}
