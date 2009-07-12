// minimized by Karl
     
char $tainted *arg;
char $untainted *zot;
     
int sprintf(char $_1_2 * str, char const $untainted * format, ... $_1);
     
void foo()
{
  char * buf;
  sprintf(buf, "%s", arg);
  zot = buf;
}
