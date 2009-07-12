// explicit instantiation of constructor

template <class C>
struct A {
    A() {}
};

template A<char>::A();

typedef A<int> Aint;
template Aint::A();
