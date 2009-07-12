# 1 "transpunion.c"
 

struct BoxedInt {
  int x;
};

typedef union {
  int *intPtr;
  struct BoxedInt *boxedPtr;
} CompatArgUnion __attribute__((__transparent_union__));

extern int compatFunc(int, CompatArgUnion);

int compatFunc(int firstArg, CompatArgUnion secondArg)
{
  return firstArg + *(secondArg.intPtr);
}


int main()
{ 
  int i = 6;
  struct BoxedInt b;
  int ret = 0;

  b.x = 7;

   
  ret += compatFunc(-6, &i);
  
   
  ret += compatFunc(-7, &b);
  
  return ret;
}


