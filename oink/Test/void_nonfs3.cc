//  No error is produced from:
char $tainted* cpy;
void $untainted * unt;

void f()
{
   unt = cpy;
}
