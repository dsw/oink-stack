int foo();
//  int foo(x)
//       int x;
//  {
//    return x;
//  }

void main(void)
{
  $tainted int y;
  int z;

  z = foo(y);
}
