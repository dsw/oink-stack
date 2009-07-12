// templatized enum parameter

// originally found in package 'gtkmathview'

// b.ii:10:9: error: cannot convert `int' to `enum MyEnum': incompatible atomic types (inst from b.ii:7:14) (inst from b.ii:10:9)

// ERR-MATCH: cannot convert `int' to `enum.*': incompatible atomic types

enum MyEnum { e1 };

template <MyEnum e>
void foo() { foo<e>(); }

int main() {
  foo<e1>();
}

