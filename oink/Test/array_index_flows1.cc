// test array indicies flowing through an array de-reference

int main() {
  int $tainted t;
  int toupper[256];
  int $untainted u;
  u = toupper[t];               // bad
  return 0;
}
