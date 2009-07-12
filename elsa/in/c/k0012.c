struct S {
    int a;
};

struct S foo () {
    struct S s;
    int i;
    s.a = i;
    return s;
}

int main()
{
    int u;
    u = foo().a;
}
