// attribute unused on struct parameter

// originally found in package 'rbldnsd_0.994b'

// Assertion failed: e != NULL && "388cba6d-895c-4acb-ac25-c28fc1c4c2cb", file cc.gr line 1234

// ERR-MATCH: 388cba6d-895c-4acb-ac25-c28fc1c4c2cb

struct S1 {};
int foo(struct S1 __attribute__((unused)) *x)
{
}
