// ERR-MATCH: designator [.].*? may only index into a compound type

// oink: compoundinit15.c:12: error: designator .b may only index into a
// compound type

// from /gnugo_3.6-2/i/gnugo_3.6-2/josekidb.c.7bda29e7b55a63f4aaa3d04cca12a12b.i

// dsw: I think gcc should not accept this; note that the embedded
// struct is not a data member

struct S1 {
  int a;
  struct {
    int b;
  };
};

struct S1 s1 = {
  42,
  { .b=43 }
};
