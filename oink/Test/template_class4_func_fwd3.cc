// test mutual resursion of function members of templatized classes
// with specialization

//  #include <iostream>
//  using namespace std;

template<class T>
struct A {
  T f(T x, T y);                // forward declaration
  T g(T x, T y);                // forward declaration
};

template<>
struct A<int> {
  int f(int x, int y);          // forward declaration
  int g(int x, int y);          // forward declaration
};

template<class T> T A<T>::f(T x, T y) {
}

template<class T> T A<T>::g(T x, T y) {
}

// FIX: I do not understand why this is not necessary and is in fact
// an error in g++ to put the 'template<>'
//
// sm: It was a bug in Elsa that it was being allowed (required?).
// It should not be there because A<int> is an already-declared
// complete specialization (this rule seems to be vaguely implied
// by 14.5.4.3).
//template<>
int A<int>::f(int x, int y) {
  if (1) {                      // cqual doesn't see this is determined at compile time
    return x;                   // bad
  } else {
    return g(0, y);
  }
}

//template<>
int A<int>::g(int x, int y) {
  if (1) {                      // cqual doesn't see this is determined at compile time
    return 17;                  // irrelevant
  } else {
    return f(y, 0);
  }
}

int main() {
  int $tainted x;
//    int x = 1;
  int x2 = x;                   // launder taintedness
  int $untainted y;
//    int y = 2;
  A<int> a;
  y = a.f(0, x2);
//    cout << "y:" << y << endl;
  return 0;
}
