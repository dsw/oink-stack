# 1 "combine_samefn_1.c"
 
 
 
 
                               
 
#pragma ccuredpoly("some_poly_fn")

 
int foo(int xxx)
{
  int yyy = xxx + 3;     
  int z = yyy + xxx;     
  return z + xxx;        
}


int myglobal __attribute__((mayPointToStack)) = 3;


 
__inline static int func()
{
  return 3;
}

__inline static int func___0();
__inline static int func___0()
{
  return 3;
}


 
int otherFunc();


int main()
{
  int ret = func() + func___0() - 6;     
  ret += foo(5) - 18 + myglobal - 3;     
  ret += otherFunc() - 3;                
  return ret;
}

