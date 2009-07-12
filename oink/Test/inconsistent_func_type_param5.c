// ERR-MATCH: 4798ba56-1da1-48ce-8371-1cfddccfd17c

// qual: a.i:7: Source function has fewer parameters before the ellipsis
// than target function (4798ba56-1da1-48ce-8371-1cfddccfd17c).

typedef int (*Func) (int a, int b);

int foo (int a, ...);

int main()
{
  (Func) &foo;
}
