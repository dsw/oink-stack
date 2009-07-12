//  #ifndef __FSEQN
//  #define __FSEQN
//  #endif

typedef struct {
    char * name;
    int data;
} cmd;

char *char_p;

int main() {
  cmd our_cmds[] = {
    { "command 1", 1 },
    { "command 2", 2 },
    { "command 3", 3 },
//      { 4, 5} };
    { char_p, 5} };

  struct {
    int x;
    cmd * cmds;
    int y;
  } main_struct = { 
    101,
    our_cmds,
    202
  };

//    char * /*__FSEQN*/ p = "HELLO";
//      return 0;
}

