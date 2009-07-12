// test omitting the then clause of a conditional expression

// In gcc it is legal to omit the 'then' part of a conditional.
// http://gcc.gnu.org/onlinedocs/gcc-3.4.1/gcc/Conditionals.html#Conditionals

int y;
// with a space
int x = y ? : 3;
// without one
int z = y ?: 3;

int main() {
  int x;
  switch(3) {
    // in an expression that should be constant evaluated.
  case 3 ? : 1:
    ;
  }
}
