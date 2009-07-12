// ERR-MATCH: 2068fccd-bce0-4634-888c-06063852a47c

// check that builtins typecheck

int __builtin_putchar(int x) {
  return x;
}
