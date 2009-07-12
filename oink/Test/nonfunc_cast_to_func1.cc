// ERR-MATCH: 9fe4294a-ac9e-4ae4-b592-86c5c9cae54f

// This may be hard to implement but at least this file helps counting the
// number of occurrences.

// qual: Can't insert edge from a non-int atomic char to function void ()()

typedef void (*MyFunc) ();

int main()
{
  char *p;
  MyFunc f = (MyFunc) p;
}
