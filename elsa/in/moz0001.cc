// moz0001.cc
// gcc builtin used as a subexpression

struct s {
  int m;
};

int myfunc(char* buf, __builtin_va_list arg)
{
  int z = __builtin_va_arg(arg, s*)->m;
}

int main()
{
}

