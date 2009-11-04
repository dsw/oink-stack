int main() {
  int a[1000];
  int i, sum;

  for (i=0; i < 1000; i = i + 1) {
    a[i] = i + 1;
  }

  sum = 0;
  for (i=0; i < 1000; i = i + 1) {
    sum += a[i];
  }

  return sum != 500500;
}
