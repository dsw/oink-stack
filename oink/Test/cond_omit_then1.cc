// test omitting the then clause of a conditional expression

// In gcc it is legal to omit the 'then' part of a conditional.
// http://gcc.gnu.org/onlinedocs/gcc-3.4.1/gcc/Conditionals.html#Conditionals

int $tainted y;
int $untainted x = y ?
  2                             // good
  : 3;
