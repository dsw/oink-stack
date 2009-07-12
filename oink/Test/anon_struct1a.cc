// test linking anonymous structs (enums have the same issue)

typedef struct {
  int x;
} S1;

int foo(S1*);

int main()
{
  return foo(0);
}
