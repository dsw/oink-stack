// check that we are visiting the declarator

int main()
{
    int q;
    int p = (int $tainted) q;
    int $untainted u;
    u = p;                      // BAD
}

