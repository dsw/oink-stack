// #ifndef QUAL
// # define $tainted
// #endif

// #ifndef QUAL
// # define $untainted
// #endif

int myprintf(char $untainted * fmt, ...) {}

extern inline char foo() {
}

// extern inline char foo() {
//     char $tainted c = 'x';
//     return c;
// }

int main()
{
    char c = foo();
    myprintf(&c);
}

