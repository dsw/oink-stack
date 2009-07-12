// ERR-MATCH: 9fe4294a-ac9e-4ae4-b592-86c5c9cae54f

// qual: Can't insert edge from a non-function char * to function void ()()

typedef void (*MyFunc) ();

int main()
{
  char **p;
  MyFunc f = (MyFunc) p;
}
