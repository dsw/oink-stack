// enum variable declared earlier as int

// originally found in package workman

// a.i:7:13: error: prior declaration of `foo' at a.i:5:12 had type `int', but this one uses `enum MyEnum'

// ERR-MATCH: prior declaration of `.*?' at .*? had type `int', but this one uses `enum .*?'

enum MyEnum { DUMMY_WHICH_MUST_BE_NEGATIVE = -1, DUMMY2 };

extern int foo;

enum MyEnum foo = DUMMY2;

// related: the unsigned version when no negative enumerators
enum E2 { blah };
extern unsigned int e2;
enum E2 e2;
