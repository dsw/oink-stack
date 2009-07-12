/*impl-int*/ x;
extern /*impl-int*/ x2;
extern /*impl-int*/ *y;             /* this is evil */
static /*impl-int*/ z;

int main() {
  x++;
  x2++;
  (*y)++;
  z++;
  int x1 = x;
  int x0;
  x0 = x2;
  x0 = 13;
}

/*impl-int*/ f(int x);

/*impl-int*/ f2(x, y)
  int x;
  /*impl-int y;*/ 
{
  x++;
  y++;
}
