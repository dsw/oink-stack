// #ifndef QUAL
// # define $tainted
// #endif

// #ifndef QUAL
// # define $untainted
// #endif

char foo() {
    char $tainted c = 'x';
    return c;
}
