// ERR-MATCH: 3ba6003d-d210-4211-b01b-59a3f41b5c0d

// from drbd0.7-utils_0.7.10-4/i/drbd_0.7.10-4/drbdsetup.c.46782ea436f87d1791f5573b0f54e4e4.i

// dsw: in my opinion gcc should not accept this

struct S1 {
  int a, b;
};

struct S2 {
  struct S1 *s1;
};

struct S2 s2[] = {
  {
    (struct S1[]) {
      { 1, 2 }
    }
  }
};
