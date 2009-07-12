// an expression is an l-value

// #include <iostream>
// using namespace std;

int main() {
  int $untainted a = 0;
  int b = 0;
  int &x = (a = b);
//   cout << "a " << a << endl;
//   cout << "b " << b << endl;
//   cout << "x " << x << endl;
  int $tainted y;
  x = y;                        // bad
//   cout << "a " << a << endl;
//   cout << "b " << b << endl;
//   cout << "x " << x << endl;
  return 0;
}
