template<class T>
struct S {
    static char a;
};

int foo()
{
    char $tainted t;

    S<int>::a = t;
}
