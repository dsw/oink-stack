//  No error is produced from:
void $tainted* cpy;
char $untainted * unt;

void f()
{
   unt = cpy;
}
