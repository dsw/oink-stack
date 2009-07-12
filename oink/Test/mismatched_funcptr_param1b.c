// ERR-MATCH: 1f04795f-4345-47a0-b7f8-dd8bb1ee4438

typedef void (*MyFunc) (int);

struct S {
  MyFunc func;
};

void foo();

int main() {
  struct S s = { foo };
}

