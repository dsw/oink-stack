
struct S {
  char * p;
};

char $tainted* getenv() {}
int printf(char $untainted *, ...) {}

void printS(void* x)
{
  printf(((struct S*) x)->p);
}

void callfunc( void(*f)(void*), void* data)
{
  (*f)(data);
}

int main()
{
  struct S * s1; s1->p = getenv();

  callfunc( (void(*)(struct S*)) printS, s1 );
}
