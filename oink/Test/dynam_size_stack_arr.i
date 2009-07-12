//  /home/dsw/ballA/./alchemist-1.0.26-1/cache-7Axk.i:5710:30: error:
//  can't const-eval non-const variable `depth'

// testing the gnu extension where stack allocated arrays are allowed
// to have their size determined at runtime

void f(int depth) {
  int a[depth];
}
