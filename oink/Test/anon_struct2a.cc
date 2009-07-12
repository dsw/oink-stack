// test linking anonymous structs (enums have the same issue)

typedef enum { e2a } E2;

int bar(E2);

int main()
{
  return bar(e2a);
}
