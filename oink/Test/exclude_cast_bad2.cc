// don't allow noncast in casts

int main() {
  int x;
  int y = static_cast<int $!noncast>(x);
}
