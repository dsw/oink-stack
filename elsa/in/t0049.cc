// cc.in49
// friend decls

class Foo {
  typedef int Integer;
  friend int f(Integer i);
};

int main()
{
  return f(3);
}
