int setX(int x0) {
  static                        // bad
    int x;
  int ret = x;
  x = x0;
  return ret;
}

int main() {
  int $tainted t;
  int $untainted u;
  setX(t);
  u = setX(3);
}
