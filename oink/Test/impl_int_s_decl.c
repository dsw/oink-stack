int main0() {
  // gcc and oink both don't like this
/* gcc says:  impl_intA_s_decl.c:4: error: `boink' undeclared (first use in this function) */
/* oink says: impl_intA_s_decl.c:2:3: error: there is no variable called `boink' */
/*    boink; */
  extern /*impl-int*/ tls_norestore;
  tls_norestore++;
  register /*impl-int*/ i;
  i++;
  static /*impl-int*/ hz2gb_inst = 0;
  hz2gb_inst++;
  typedef int Gpm_Event;
  extern /*impl-int*/ gpm_convert_event(char *data, Gpm_Event *event);
  gpm_convert_event(0, 0);
  if (1) {
    extern /*impl-int*/ gpm_convert_event2(char *data, Gpm_Event *event);
    gpm_convert_event2(0, 0);
  }
  static /*impl-int*/ tf_gets(), tf_read();
  int x = tf_gets();
  int y = tf_read();
  static /*impl-int*/ buf[8192];
  int z = buf[3];
  static /*impl-int*/ x1, y1;
  ++x1;
  ++y1;
  register /*impl-int*/ x9, y9;
  x9++;
}

struct A {
  struct A *next;
};

void f() {
  struct A *p;
  while (p) {
    // A place where p was being parsed as impl-int erroneously
    // previously
    p = p->next;
  }
}

// test S_decl and param impl-int don't get confused

int *x;
int main1() {
  f(&x);
}

int *x2;
int main2() {
  f2(*x2);
}

int *x3;
typedef int f3;
int main3() {
  f3(*x3);
}
