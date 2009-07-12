// /home/ballAruns/tmpfiles/./lftp-2.6.3-3/ftpclass-n5uO.i

int main() {
  char *pwd=(char*)__builtin_alloca(3);
}

// quarl 2006-09-07
//    Do NOT declare __builtin_alloca since it won't be compatible with the
//    FF_NOPARAM in gnu.cc.  (After builtin-declarations.h is used, this
//    should be ok.)  I didn't see any real code that does so.

// /home/ballB/gcc-2.96-110/bb-reorder-Zb4y.i
// extern void *__builtin_alloca (unsigned int);

// try it again
int f() {
  char *pwd=(char*)__builtin_alloca(3);
}
