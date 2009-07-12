// test forwarded class templates in the presence of specializations

// forwarded class template primary
template<class T> struct A;

// forward specialization
template<class T2> struct A<T2*>;

// define specialization
template<class T2> struct A<T2*> {
  int foo(int x) {
    return x;                   // bad
  }
};

// an instantiation that matches the specialization
A<int*> a;

// the definition of the primary
template<class T> struct A {
  int foo(int x) {
    // return x;
  }
};

int main() {
  int $tainted x;
  int $untainted y;
//    int x;
//    int y;
  y = a.foo(x);
}
