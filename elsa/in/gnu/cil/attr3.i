# 1 "attr3.c"
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








 


































# 1 "attr3.c" 2
 
















 

int __attribute__((a( 6 )))   * __attribute__((a( 5 )))   (__attribute__((a( 3 )))   * __attribute__((a( 2 )))   (__attribute__((a( 1 )))   x1)[5])(int __attribute__((a( 4 )))  ) __attribute__((name)) ;


 

extern int __attribute__((a( 7 )))   * __attribute__((a( 6 )))   (__attribute__((a( 4 )))   x2)(float __attribute__((a( 5 )))   x) __attribute__((name)) ;

 


int __attribute__((a( 7 )))   * __attribute__((a( 6 )))   (__attribute__((a( 4 )))   * __attribute__((a( 3 )))   (__attribute__((a( 1 )))   x3)(int __attribute__((a( 2 )))   x))(float __attribute__((a( 5 )))  ) {
  return & x2;
}
