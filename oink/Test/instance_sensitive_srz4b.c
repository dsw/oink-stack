void foo(void * p) {
  char $tainted c;
  *(  (char*) p ) = c;
}
