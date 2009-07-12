struct S {
    int a;
};

struct S foo () {
    struct S s;
    int $tainted i;
    s.a = i;
    return s;
}

int main()
{
    int $untainted u;
    u = foo().a;                                    // bad
}
