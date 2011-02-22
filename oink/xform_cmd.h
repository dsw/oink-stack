// see License.txt for copyright and terms of use

// Process command-line arguments.

#ifndef XFORM_CMD_H
#define XFORM_CMD_H

#include "oink_cmd.h"
#include "oink_file.h"          // File

class XformCmd : public virtual OinkCmd {
  public:
  // print out every declaration allocating a var on the stack
  bool print_stack_alloc;
  // print out every declaration (1) allocating a var on the stack
  // where (2) the var also has its address taken
  bool print_stack_alloc_addr_taken;
  // heapify every declaration (1) allocating a var on the stack where
  // (2) the var also has its address taken

  bool heapify_stack_alloc_addr_taken;
  // insert calls to verify the status of parameters that are pointers
  // to a class/struct/union type that is defined in this module
  bool verify_cross_module_params;
  // localize calls to heap allocation calls: change calls to
  // malloc/free etc. so that they call class-local and module-local
  // malloc
  bool localize_heap_alloc;
  // introduce function calls at the call site
  bool intro_fun_call;
  bool wrap_fun_call;

  // jimmy stage
  bool jimmy;

  // string to use to call free()
  const char *free_func;
  // string to use to call xmalloc()
  const char *xmalloc_func;
  // string to use to call verify()
  const char *verify_func;
  // string with which to suffix altered parameters
  const char *verify_param_suffix;
  // the string to introduce function calls with
  const char *intro_fun_call_str;
  const char *intro_fun_ret_str;
  const char *wrap_fun_call_config_file;
  XformCmd();

  void readOneArg(int &argc, char **&argv);
  void dump();
  void printHelp();
  void initializeFromFlags();
};

#endif  // XFORM_CMD_H
