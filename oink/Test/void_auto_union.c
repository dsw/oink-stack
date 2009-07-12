int printf(char $untainted *fmt, ...) {}

int foo(void $_1_2 * p, char $_1 dummy)
{
}

int main()
{
    char s[100];
    char $tainted c;
    foo(s, c);                                      // bad
    printf(s);
}
