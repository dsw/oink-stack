// don't allow nonglobal qualifier in global

void f() {
  static int $!nonglobal * x;          // this is also a global
}
