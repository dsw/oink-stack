// try out other kinds of declarations

// gcc/cqual even allow the wrong number of arguments!
int gronk(x, y);

int gronk(x, y, z)
float x;
int y;
double z;
{
  return y;
}

int main() {
  int q = gronk(1.2, 4, 4.4);
  return 0;
}
