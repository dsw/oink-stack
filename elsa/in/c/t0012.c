// t0012.c
// generalized ?: lvalues

void f(int x)
{
  int i,j,k;

  __elsa_checkType(x? i : j, k);       // lval
  __elsa_checkType(x? (i+1) : j, 1);   // rval
  __elsa_checkType(x? i : (j+1), 1);   // rval
}
