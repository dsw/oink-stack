struct foo {};
void f(struct foo **x) {}
int main() {
  f( ((void*)0) );
}
