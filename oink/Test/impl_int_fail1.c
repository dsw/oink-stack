// this should fail because the first typedef makes x a variable, the
// second becomes *not* an implicit int definition for a variable x,
// but just a useless type thingy, and therefore the x++ tries to
// increment a type which is bad

typedef int x;
extern x;
int main() {
  x++;                          // error; that is, x was a type
}

