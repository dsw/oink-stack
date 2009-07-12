template<class T>
struct S {
    static char a;
};

int bar()
{
    char $untainted u;

    u = S<int>::a;    
}
