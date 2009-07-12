// old-style ellipsis

// oink: a.i:5: __builtin_va_start in function that does not have an ellipsis
// a.i.19

// ERR-MATCH: __builtin_va_start in function that does not have an ellipsis

void foo (char *, ...);
void foo (s) char *s;
{
    __builtin_va_list ap;
    __builtin_va_start(ap,s);
}
