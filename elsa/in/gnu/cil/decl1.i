# 1 "decl1.c"
struct timeval {
	int	tv_sec;		 
	int	tv_usec;	 
};

extern struct timeval xtime;


volatile struct timeval xtime __attribute__ ((aligned (16)));

extern void printf(char *, ...);



int main() {
  if((int)&xtime & 0xF != 0) { printf("Error %d\n",  1 ); return  1 ; } ;

  printf("Success\n");
  return 0;
}
