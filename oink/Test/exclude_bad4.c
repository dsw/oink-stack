// don't allow nonglobal qualifier in global

struct A {
  int x;                        // not a global
  static int $!nonglobal * y;   // this is also a global
};
