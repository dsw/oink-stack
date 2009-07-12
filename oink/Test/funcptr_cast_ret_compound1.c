// ERR-MATCH: f66eefd9-fad0-4b47-b57f-558b5f877f67

struct S1 {};

void func1 (int x) {
}

int main() {
  (struct S1 (*) (int)) &func1;
}
