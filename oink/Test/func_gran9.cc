// for testing the function granularity CFG computation

typedef void (*func_t)();

void functionptr_notcalled() {}
void foo(func_t f1, func_t f2);
void functionptr1() {}
void functionptr2() {}

int main() {
  foo(functionptr1, &functionptr2);
}
