// just test that pointers to members work as 'point type's

struct A {};

struct C {
  void (A::*x) (int);
  int *y;
};

C e[] = {
  {0, 0}
};
