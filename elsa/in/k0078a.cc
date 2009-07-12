// array of function pointers initialized with template functions

// originally found in package 'monotone_0.18-1'

// d.ii:13:5: error: there is no action to merge nonterm InitializerList

// ERR-MATCH: error: there is no action to merge nonterm InitializerList

template <class T1, class T2>
struct S {
  static bool foo();
};

typedef bool (*FuncType)();

template <class T1, class T2>
void bar() {
  FuncType funcs[] = {
    &S<T1,T2>::foo
  };
}
