// confusion between function template decl and return type

// reported by Evan Driscoll <driscoll@cs.wisc.edu>

// a.cc:6:1: error: prior declaration of class C at a.cc:1:1 was not
// templatized, but this one is, with parameters template <class T>

class C
{
};

// parse error goes away if "class" removed
template <class T>
class C foo(T t)
{
    C c;
    return c;
}

int main()
{
    C c = foo<int>(5);
}
