// compoundinit5Bw.c; see License.txt for copyright and terms of use

void funb() {
  struct foo2 {
    struct bar2 {
      wchar_t a2[10];
    } b2;
  };
  struct foo2 f2 = {
    L'ha', L'ea', L'la', L'la', L'oa'
  };
}
