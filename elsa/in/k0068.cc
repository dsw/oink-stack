// explicitly named constructor

// k0068.cc:9:3: error: there is no variable called `C::C'
// typechecking results:
//   errors:   1
//   warnings: 0

// ERR-MATCH: error: there is no variable called `([A-Za-z0-9_]+)::\1'

struct C {
  C (int x) {}
};

int main()
{
  C::C(42);
}
