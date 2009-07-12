// t0014.c
// type of character literals

typedef long wchar_t;

void foo()
{
  __elsa_checkType('a', (int)0);
  __elsa_checkType('ab', (int)0);
  __elsa_checkType(L'a', (wchar_t)0);
}
