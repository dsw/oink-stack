// compoundinit5Aw.c; see License.txt for copyright and terms of use

// test assigning to arrays of characters
void funa() {
  struct foo {
    struct bar {
      wchar_t a[10];
    } b;
  };
  struct foo f = {
    L"hello"
  };
}
