// do we find a pointer to a stack stored into a global?

int *g;

int main() {
  int s;
  g = &s;
  return 0;
}
