int x;

void setX(int x0) {
  x = x0;
}

int main() {
  int $tainted t;
  int $untainted u;
  setX(t);
  u = x;                        // bad
}
