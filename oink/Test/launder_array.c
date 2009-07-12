char a[100];
int main(int argc, char **argv) {
  char i;
  for (i='a'; i<'z'; ++i) a[i] = i;
  char $tainted x = argv[0][0];
  char $untainted y = a[x];
  return 0;
}
