// t0533.cc
// double-typed expr

void f(double,double);     // line 4: this is the right one
void f(int,int);

void foo()
{
  double d;
  f(2*d+4, 3.0);           // calls 'f' on line 4
  f(4+2*d, 3.0);           // calls 'f' on line 4

  __elsa_checkType(2*d+4, (double)0);
  __elsa_checkType(4+2*d, (double)0);
  
  char *p;
  int i;
  
  __elsa_checkType(p+i, (char*)0);
  __elsa_checkType(p-i, (char*)0);
  __elsa_checkType(p-p, (int)0);
}
