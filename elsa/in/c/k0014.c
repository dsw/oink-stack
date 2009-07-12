// offsetof cannot be assumed to be 0 (evaluated e.g. in case labels)

// originally found in package ''

// a.i:11:5: error: division by zero

// ERR-MATCH: division by zero

struct S {
  int foo1;
  int foo2;
};

int main() {
  switch (42) {
  case 100 / ((int) & ((struct S*) 0)->foo2):
    break;
  }
};
