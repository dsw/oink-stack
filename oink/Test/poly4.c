
int foo(int x) { return x; }

int main()
{
    int $tainted t;
    int $untainted u;

    u = foo(t);                 // BAD
}
