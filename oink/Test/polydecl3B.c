void foo($_1 int *x);
//  {
//  }

void main(void)
{
  $tainted int a;
  int b;

  foo(&a);
  foo(&b);
}
