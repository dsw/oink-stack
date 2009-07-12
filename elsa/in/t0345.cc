// t0345.cc
// types of integer literals

void foo()
{
  __elsa_checkType(1, (int)0);
  __elsa_checkType(1U, (unsigned)0);
  __elsa_checkType(1L, (long)0);
  __elsa_checkType(1UL, (unsigned long)0);
  __elsa_checkType(1LL, (long long)0);
  __elsa_checkType(1ULL, (unsigned long long)0);

  // the following tests are dependent on the host architecture, so I
  // have them commented-out; they are correct for a 32-bit int
  //
  // actually, the int rolls over to long long if int and long are
  // the same size ...

  // decimal: int -> long
  //__elsa_checkType(2147483647, (int)0);
  //__elsa_checkType(2147483648, (long long)0);

  // hex: int -> unsigned
  //__elsa_checkType(0x7FFFFFFF, (int)0);
  //__elsa_checkType(0x80000000, (unsigned)0);
}


// EOF
