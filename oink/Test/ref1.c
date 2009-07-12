// FIX: I think this one is wrong.  Doesn't unify y and x.
int main() {
  int $tainted x;
  int &y = x;
}
