
typedef struct {
  int x;
} * const *S1;

static int foo(S1*) {}

int main()
{
  return foo(0);
}
