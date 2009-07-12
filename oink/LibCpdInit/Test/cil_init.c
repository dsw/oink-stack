//  #ifndef __NULLTERM
//  #define __NULLTERM
//  #define __SIZED
//  #endif
//  #include "testharness.h"

//  extern int strcmp(const char*, const char*);

/* run this with COMPATMODE=1 if compiling directly, since neither GCC nor 
 * MSVCC fully support the C standard */
//  static char *usageplocal = "Usage";
static char usageescape; // = 'C';

//  char *usagep = "Usage non-local";
//  char *usagep1 = { "Usage in a brace" };
char usages[] = "Usage string";
void dummy0() {
  int  g = { 6 } ;
//    char strange[] = { "several" };
}

//  char *null = (void*)0;


typedef struct s {
  char *name;
  int   data;
} STR;

char *dummy_charp;
char *dummy_charp2;

//  extern int afunc(int x);
//  int (*fptr)(int) = afunc;

void dummy1() {
  STR a[] = {
    {"first", 5},
    {"second", 1},
//      {& usages[2], 2},
    {dummy_charp, 2},
//      { & usageescape, 3},
    { dummy_charp2, 3},
    { usages, 4},
  };
}


typedef struct {
  struct {
    char * a1[10];
    char * a2;
    char   strbuff[20]/* __NULLTERM*/;
  } f1;
  struct {
    int * i1;
  } f2[5]/* __SIZED*/;
} NESTED;

NESTED glob1;

//  int glob3;
//  int * glob2 = & glob3;

int afunc(int a) {
  NESTED loc1;
  char   locbuff[30]/* __NULLTERM*/;
  char   indexbuff[10]/* __SIZED*/;

//    loc1.f1.a2 = glob1.f1.a2;
  
//    return * loc1.f2[3].i1 + (locbuff[0] - indexbuff[0]);
}



// now initialization for union
void dummy2() {
  union  {
    struct {
      int a;
//        int *b;
      int b;
    } u1;
    int c;
  } uarray[] = { 1, 11, 2, 12, 3, 13 };
}


void dummy3() {
  // now some examples from the standard
  int z[4][3] =
  { { 1 }, { 2 }, { 3 }, { 4 } };

  struct str1 { int a[3]; int b;};

  struct str1 w[] =
  { { 1 }, { 2 } };


  short q[4][3][2] = {
    { 1 } ,
    { 2, 3 },
    { 4, 5, 6}
  };

  short q1[4][3][2] = {
    1, 22, 11, 12, 13, 14,
    2, 3, 15, 16, 17, 18,
    4, 5, 6, 19, 22, 21,
  };



  //  #ifdef _GNUCC
  int a1[10] = {
    1, 3, 5, 7, 9, [6] = 8, 6, 4, 2};


  enum { member_one, member_two, member_three };
  char *nm[] = {
    [member_two] = "member_two",
    [member_three] = "member_three",
  };
}

//  #endif



//  #define ERROR(n) { printf("Incorrect init: %d\n", n); exit(1); }
// Test the initialization
//  int main() {
//    int i;

//    struct str1 astr = w[0];
    
//    if(strcmp(a[0].name, "first")) {
//  //      ERROR(0);
//    }
//    if(sizeof(uarray) / sizeof(uarray[0]) != 3) {
//  //      ERROR(1);
//    } 
//    if(uarray[2].u1.a != 3) {
//  //      ERROR(2);
//    }

//    if(z[2][0] != 3 ||
//       z[2][1] != 0) {
//  //      ERROR(4);
//    }

//    if(sizeof(w) / sizeof(w[0]) != 2 ||
//       w[1].a[0] != 2) {
//  //      ERROR(5);
//    }
//    {
//      short * ps = (short*)q, * ps1 = (short*)q1;
//      for(i=0;i<sizeof(q) / sizeof(short); i++, ps ++, ps1 ++) {
//        if(*ps != *ps1) {
//  //          ERROR(6);
//        }
//      }
//    }

//  #ifdef _GNUCC
//    if(a1[1] != 3 ||
//       a1[5] != 0 ||
//       a1[6] != 8 ||
//       a1[7] != 6) {
//  //      ERROR(7);
//    }


//    if(strcmp(nm[1], "member_two") ||
//       strcmp(nm[2], "member_three") ||
//       sizeof(nm) != 3 * sizeof(nm[0])) {
//  //      ERROR(8);
//    }

//  #endif

  
//    printf("Initialization test succeeded\n");
//    return 0;
//  }



