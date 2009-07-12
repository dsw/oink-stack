// don't allow noncast in casts

int main() {
  int x;
  int y = (int $!noncast) x;
}
