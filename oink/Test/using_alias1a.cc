namespace N1 {
  extern int identity(int i);
}

using N1::identity;

int main()
{
  int $tainted t;
  int $untainted u;

  u = identity(t);              //BAD
}
