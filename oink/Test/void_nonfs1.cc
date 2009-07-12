//  No error is produced from:
void $tainted* cpy;
void $untainted * unt;

void f()
{
   unt = cpy;
}
