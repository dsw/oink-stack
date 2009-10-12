
struct astruct {
 int myarray[20];
};
int i = __builtin_offsetof (astruct, myarray[0]);
