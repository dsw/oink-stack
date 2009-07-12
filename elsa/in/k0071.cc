// compound initializer for struct which was earlier forward-declared as class

// originally found in package 'kdelibs'

// a.ii:8:3: error: cannot convert initializer type `int' to type `struct S'

// ERR-MATCH: cannot convert initializer type `.*?' to type `struct .*?'

class S;
struct S {
  int foo, bar;
};

S s = { 42, 84 };
