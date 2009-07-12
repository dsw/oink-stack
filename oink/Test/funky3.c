struct FILE;
char $_1 * fgets(char $_1* s, int size, struct FILE *stream);
int main() {
  char line[512] ;
//    char *line;
  struct FILE *f ;
  char $tainted *tmp___0 ;
  tmp___0 = fgets(line, (int )sizeof(line), f);
}
