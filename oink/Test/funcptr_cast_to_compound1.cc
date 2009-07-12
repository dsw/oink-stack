struct S {};

typedef void (*funcptr)(void);

int main()
{
  S * s;
  funcptr f = (funcptr) s;
}
