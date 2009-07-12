// ERR-MATCH: 5b1d3a6a-1520-4765-95d4-3f53516f4e13

// from elsa/in/t0164.cc

struct Foo {
  struct Bar {
    Bar(Bar const &o) {}
  };

  Bar b;
};

int main()
{
  Foo *pf1;
  Foo f2(*pf1);
}
