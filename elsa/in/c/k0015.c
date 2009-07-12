// variable-sized character buffer in function-local struct using same name
// for size as a member variable and another variable in outside scope

// originally found in package 'rng-tools_2-unofficial-mt.10-1'

// a.i:10:19: error: can only use 'this' in a nonstatic method

// ERR-MATCH: can only use .this. in a nonstatic method

int main()
{
  int size = 42;

  struct S {
    int size;
    unsigned char data[size];
  };
}
