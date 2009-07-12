int return_tainted();
void want_untainted(int $untainted fmt, ...);
int main() {
  int a = return_tainted();
  want_untainted(a);
}
