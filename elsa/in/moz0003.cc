// moz0003.cc
// bitlength-reducing cast in integral constant expression
// should work even when Elsa is compiled on a 64-bit machine

int a[(unsigned int) -1 == 4294967295U];
