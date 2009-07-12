// -cc-catch-quals should provide the qualifier for the generic global
// exception
int main() {
  int $tainted y;
  throw y;
}
