// test with -fo-instance-sensitive.

typedef struct MYNODE *MYLIST;
struct MYNODE
{
  struct MYNODE * *member;
  int foo;
} MYNODE;

MYLIST xmalloc() {}

MYLIST getlist ();

void suck ()
{
  MYLIST x = getlist();
  x->member;
  int $untainted bar = x->foo;
}

MYLIST getlist ()
{
  MYLIST root = xmalloc ();
  root->member[0] = xmalloc ();
  root->member[0]->foo;
  int $tainted t;
  root->member[0]->foo = t;                       // BAD
  return root;
}

int main() {}

