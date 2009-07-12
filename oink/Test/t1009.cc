// array = *
int main() {
  int x [] $untainted;
  int * $tainted z;
  x = z;                        // bad
}
