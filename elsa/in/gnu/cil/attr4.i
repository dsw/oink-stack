# 1 "attr4.c"
# 1 "/slack8/home/scott/wrk/safec/cil-trunk/include/ccured.h" 1
 

 




 




# 1 "/slack8/home/scott/wrk/safec/cil-trunk/include/ccuredannot.h" 1
 

 










# 27 "/slack8/home/scott/wrk/safec/cil-trunk/include/ccuredannot.h"

  


  
  
  
  
  


 





 













 









   
  
   
  
   
  
   
  
   
  




 












extern int __ccuredAlwaysStopOnError;
 




extern int __ccuredUseStrings;
 



extern int __ccuredLogNonPointers;
 


extern int __ccuredDisableStoreCheck;
 


extern void* __ccuredStackBottom;
 



extern void __ccuredInit(void);

  #pragma cilnoremove("__ccuredInit")

 

  void ccured_fail_str(char *str,
                                    char* file, int line, char *func);
  void ccured_fail_str_terse(char *str);
 

 






  void ccured_fail(int msgId, char* file, int line, char *func);
  void ccured_fail_terse(int msgId);
 
 






 



  void non_pointer_fail(unsigned long l,
                                     char* file, int line, char *func);
  void non_pointer_fail_terse(unsigned long l);



 


 


void __logScalar(int id, unsigned long l);








 











































  






















 
 








































 


































# 13 "/slack8/home/scott/wrk/safec/cil-trunk/include/ccured.h" 2




 

extern void* wrapperAlloc(unsigned int);
#pragma cilnoremove("wrapperAlloc")
#pragma ccuredalloc("wrapperAlloc", sizein(1), nozero)

extern void wrapperFree(void *);
#pragma cilnoremove("wrapperFree")
#pragma ccuredpoly("wrapperFree")

 
extern char* wrapperStrdup(char *);
#pragma cilnoremove("wrapperStrdup")
#pragma ccuredpoly("wrapperStrdup")

   
unsigned __ccured_mult_u32(unsigned x, unsigned y);



 
 
 
 


 
 
 
 
 
 
 
#pragma ccuredpoly("__ptrof_nocheck")
#pragma cilnoremove("__ptrof_nocheck")
void *  __attribute__((safe))     __ptrof_nocheck(void *ptr);
 
 
 

#pragma ccuredpoly("__startof")
void *  __attribute__((safe))     __startof(void *ptr); 
 
 
 

#pragma ccuredpoly("__endof")
void *  __attribute__((safe))     __endof(void *ptr);
 
 
 
 

 



 



#pragma ccuredpoly("__ptrof")
void *  __attribute__((safe))     __ptrof(void *ptr);
 
 
 

#pragma ccuredpoly("__verify_nul")
void __verify_nul(char const *ptr);
 
 
 
 



#pragma ccuredpoly("__strlen")
int __strlen(char *ptr);      
 
 
 
 
 
 


#pragma ccuredpoly("__strlen_n")
int __strlen_n(char *ptr, int n);
 
 
 
 
 
 
 
 
 
 


#pragma ccuredpoly("__stringof")
char * __stringof(char const *ptr);
#pragma cilnoremove("__stringof")
 
 
 
 
 
 
 

#pragma ccuredpoly("__stringof_ornull")
char * __stringof_ornull(char const *ptr);
#pragma cilnoremove("__stringof_ornull")
 
 
 
 
 
 
 

#pragma ccuredpoly("__write_at_least")
void __write_at_least(void *ptr, unsigned int n);  
 
 
 
 
 

#pragma ccuredpoly("__read_at_least")
void __read_at_least(void *ptr, unsigned int n);  
 
 
 
 

#pragma ccuredpoly("__copytags")
void __copytags(void *dest, void* src, unsigned int n);  
 
 
 
 
 
 
 
 


 
 
 
 
 

#pragma ccuredpoly("__mkptr")
#pragma cilnoremove("__mkptr") 
void * __mkptr(void *  __attribute__((safe))    p, void *phome);
 
   
   
   

#pragma ccuredpoly("__mkptr_int")
void * __mkptr_int(unsigned long p, void *phome);
 
 
 
 
 

#pragma ccuredpoly("__mkptr_size")
#pragma cilnoremove("__mkptr_size")
void * __mkptr_size(void *  __attribute__((safe))    p, unsigned int len);
 
 
 
 
 
 

#pragma ccuredpoly("__mkptr_string")
#pragma cilnoremove("__mkptr_string")
char * __mkptr_string(char *  __attribute__((safe))    p);
 
 
 
 


#pragma ccuredpoly("__align_seq")
#pragma cilnoremove("__align_seq")
void* __align_seq(void *p, unsigned int size);
 
 
 
 
 


 

#pragma ccuredpoly("__trusted_cast")
#pragma cilnoremove("__trusted_cast")
void * __trusted_cast(void * p);


   
   
   





#pragma ccuredpoly("ccured_hasuniontag")
int  ccured_hasuniontag(void *);

 
 



int  __ccured_kind_of(void *);
#pragma ccuredpoly("__ccured_kind_of")
 

   
   
  
  
  
  
  
  
  
  
  
  
  
  

  
  
  
  

  
  
  
  
  
  
  
  

  

   
   
  
   
   
  



    
    
  

char* __ccured_mangling_of(unsigned int);
#pragma ccuredpoly("__ccured_mangling_of")
#pragma cilnoremove("__ccured_mangling_of")
 

int  __ccured_has_empty_mangling(unsigned int);
#pragma ccuredpoly("__ccured_has_empty_mangling")
#pragma cilnoremove("__ccured_has_empty_mangling")






 






#pragma cilnoremove("abort_deepcopy")
__attribute__((noreturn))  void abort_deepcopy(char * errmsg);

 






 






 




 




 



      

# 353 "/slack8/home/scott/wrk/safec/cil-trunk/include/ccured.h"

 












 










 








 







 





 










 


# 423 "/slack8/home/scott/wrk/safec/cil-trunk/include/ccured.h"



 





 
#pragma cilnoremove("struct printf_arguments")
struct printf_arguments {
  int i;
  double d;  
  char *  __attribute__((rostring))    s;



  long long ll;

};








 


































# 1 "attr4.c" 2
# 1 "testharness.h" 1
extern int printf(const char * format, ...);
#pragma ccuredvararg("printf", printf(1))

extern void exit(int);

 



# 1 "attr4.c" 2


typedef struct {
  int f1;
  char f1pad;
  int f2  __attribute__((packed)), f3 __attribute__((packed));
  char f2pad;
  int f4, f5 __attribute__((packed));  
  char f3pad;
  int __attribute__((packed)) f6, f7;  
} STR;





int main() {
  printf("Offset 1 = %d\n", ((int)(&((  STR *)(0))-> f1 )) );
  printf("Offset 2 = %d\n", ((int)(&((  STR *)(0))-> f2 )) );
  printf("Offset 3 = %d\n", ((int)(&((  STR *)(0))-> f3 )) );
  printf("Offset 4 = %d\n", ((int)(&((  STR *)(0))-> f4 )) );
  printf("Offset 5 = %d\n", ((int)(&((  STR *)(0))-> f5 )) );
  printf("Offset 6 = %d\n", ((int)(&((  STR *)(0))-> f6 )) );
  printf("Offset 7 = %d\n", ((int)(&((  STR *)(0))-> f7 )) );

  if(((int)(&((  STR *)(0))-> f1 ))  != 0 ||
     ((int)(&((  STR *)(0))-> f2 ))  != 5 ||
     ((int)(&((  STR *)(0))-> f3 ))  != 9 ||
     ((int)(&((  STR *)(0))-> f4 ))  != 16 ||
     ((int)(&((  STR *)(0))-> f5 ))  != 20 ||
     ((int)(&((  STR *)(0))-> f6 ))  != 25 ||
     ((int)(&((  STR *)(0))-> f7 ))  != 29) {
    return 1;
  }

  return 0;
}
