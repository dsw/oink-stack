// function pointer dependent type

// ERR-MATCH: found dependent type `[(]dependent[)]' in non-template

// // ERR-MATCH: (0a257264-c6ec-4983-95d0-fcd6aa48a6ce|ee42ebc5-7154-4ace-be35-c2090a2821c5)

// error: a.ii:11:3: internal error: found dependent type `(dependent)' in
// non-template (0a257264-c6ec-4983-95d0-fcd6aa48a6ce)

// NOTE: "cout << endl" hits this bug.

template <typename T>
struct S {
  void bar(void (*pf)(S<T>&)) {}
};

template<typename T>
void foo(S<T>& s) {}

int main() {
  S<char> s;
  s.bar(foo);
}
