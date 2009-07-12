typedef struct { } FILE;
typedef unsigned long size_t;

size_t fread(void $tainted *ptr, size_t size, size_t nmemb, FILE *stream)
{
}

FILE *stdin = 0;

struct S
{
  int a;
};

int main()
{
  struct S s;
  int $untainted u;

  fread(&s, sizeof(s), 1, stdin);

  u = s.a;
}
