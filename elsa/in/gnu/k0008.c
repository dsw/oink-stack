// gcc accepts this in C, but not C++

extern int foo();

extern inline int foo()
{
}

int foo()
{
}
