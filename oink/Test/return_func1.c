int foo() {}

typedef int (*intfunc_t)();

intfunc_t bar() {
    return & foo;
}
