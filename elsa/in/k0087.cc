// implicit conversion to "bool const &" via "bool" via "operator bool"

// originally found in package 'config-manager_0.1p83-1'

// c.ii:13:3: error: no viable candidate for function call; arguments:
//   1: struct MyBool &
//  original candidates:
//   c.ii:3:6: void foo(bool const &)
//   c.ii:4:6: void foo()

// ERR-MATCH: no viable candidate for function call; arguments

void foo(bool const &);
void foo();

struct MyBool {
  operator bool () const{}
};

int main()
{
  MyBool b;
  foo(b);
}
