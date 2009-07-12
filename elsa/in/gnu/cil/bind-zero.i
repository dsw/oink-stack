# 1 "bind-zero.c"
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








 


































# 1 "bind-zero.c" 2
 
 
 
typedef long long __quad_t;
typedef long __off_t;
typedef __quad_t __loff_t;
typedef __loff_t __off64_t;
typedef struct _IO_FILE FILE;
typedef void _IO_lock_t;
struct _IO_FILE
{
  int _flags;
  char *_IO_read_ptr;
  char *_IO_read_end;
  char *_IO_read_base;
  char *_IO_write_base;
  char *_IO_write_ptr;
  char *_IO_write_end;
  char *_IO_buf_base;
  char *_IO_buf_end;
  char *_IO_save_base;
  char *_IO_backup_base;
  char *_IO_save_end;
  struct _IO_marker *_markers;
  struct _IO_FILE *_chain;
  int _fileno;
  int _blksize;
  __off_t _old_offset;
  unsigned short _cur_column;
  signed char _vtable_offset;
  char _shortbuf[1];
  _IO_lock_t *_lock;
  __off64_t _offset;
  void *__pad1;
  void *__pad2;
  int _mode;
  char _unused2[(int) (15U * sizeof (int) - 2U * sizeof (void *))];
};
extern void * __attribute__ ((__safe__)) __ptrof_nocheck (void *ptr);
__inline static char *
fgets_wrapper (char *buf, int size, FILE * fp)
{
  char *res;
  FILE *tmp;
  char *tmp___0;
  char *tmp___1;
  {
     
    tmp = (FILE *) __ptrof_nocheck ((void *) fp);

//    res = fgets ((char *) tmp___0, size, (FILE *) tmp);

    return (tmp___1);
  }
}
char *
gets_wrapper (char *buffer)
{
  char *res;
  FILE *tmp;
  void * __attribute__ ((__safe__)) tmp___0;
  void * __attribute__ ((__safe__)) tmp___1;
  char *tmp___2;
  char *tmp___3;
  {
//    tmp = get_stdin ();
     
    tmp___1 = __ptrof_nocheck ((void *) buffer);
    tmp___2 =
      fgets_wrapper (buffer,
		     (int) ((unsigned int) tmp___0 - (unsigned int) tmp___1),
		     tmp);
    res = tmp___2;
    if ((unsigned long) res != (unsigned long) ((void *) 0))
      {
	tmp___3 = res;
	while (1)
	  {
	    if ((int) (*tmp___3) != 10)
	      {
		if (!((int) (*tmp___3) != 0))
		  {
		    break;
		  }
	      }
	    else
	      {
		break;
	      }
	    tmp___3++;
	  }
	(*tmp___3) = '\0';
      }
    return (res);
  }
}
