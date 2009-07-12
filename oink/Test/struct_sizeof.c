int main() {
  struct a {
    int a0;                     // in C++, can't name it "a"
    int b0;
  };
  int x[sizeof (struct a)];
}
