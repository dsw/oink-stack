// t0346.cc
// type of character literals

void foo()
{
  __elsa_checkType('a', (char)0);
  __elsa_checkType('ab', (int)0);
  __elsa_checkType(L'a', (wchar_t)0);
}

// EOF
