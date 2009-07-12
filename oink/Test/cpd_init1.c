struct command {
   char *name ;
   char *syntax ;
   char state_needed ;
   char showinfeat ;
};
struct command  const  commands[]  = 
  {      {(char *)"USER", (char *)"<sp> username", (char)0, (char)0}, 
  };
