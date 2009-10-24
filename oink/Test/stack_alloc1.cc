// test if we can tell what variables are allocated on the stack

int x;                          // global: no

struct Foo {
  int q;                        // class: no, as depends on container
  static int r;                 // static class == global: no
};

int f(int x) {                  // paramter: yes
  int y;                        // function/auto: yes
  static int z;                 // function/static == global: no
  ++x;                          // param use
  ++y;                          // auto use
}

template<class T>               // template param: no
class Quux {
  T w;                          // class: no
};

Quux<int> Q1;                   // global: no

template<class T>               // template param: no
T gronk(T a) {                  // param: yes
  T b;                          // function/auto: yes
  b;                            // auto use
  return a;                     // param use
}

void blort() {
  gronk(3);                     // instantiate gronk
}

namespace Bar {
  int v;                        // namespace: no
};
