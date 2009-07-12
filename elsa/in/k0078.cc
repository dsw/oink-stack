// array of function pointers initialized with template functions

// originally found in package 'monotone_0.18-1'

// Assertion failed: list != NULL && "8706f04b-0a31-407e-b31b-40607d31edf9", file cc.gr line 1437

// ERR-MATCH: Assertion failed:.*8706f04b-0a31-407e-b31b-40607d31edf9

template <class T1, class T2>
struct S {
  static bool foo();
};

typedef bool (*FuncType)();

template <class T1, class T2>
void bar() {
  FuncType funcs[] = {
    0,
    &S<T1,T2>::foo
  };
}
