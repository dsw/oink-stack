// test forwarding of function members of templatized classes
// In this version, the function definition comes before the instantiation

//  #include <iostream>
//  using namespace std;

template<class T>
struct A {
  T f(T x);                     // forward declaration
};

// definition
template<class T>
T A<T>::f(T x) {
//    cout << "f body; x:" << x << endl;
  return x;                     // bad
}

int main() {
  int $tainted x;
//    int x = 1;
  int x2 = x;                   // launder taintedness
  int $untainted y;
//    int y = 2;
  A<int> a;
  y = a.f(x2);
//    cout << "y:" << y << endl;
  return 0;
}
