// ERR-MATCH: Un-satisfied symbol .*operator (new|delete)

int main()
{
  int *p;

  p = new int;
  delete p;

  void *q;
  q = operator new(sizeof(int));
  operator delete(q);
}
