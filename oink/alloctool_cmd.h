// see License.txt for copyright and terms of use

// Process command-line arguments.

#ifndef ALLOCTOOL_CMD_H
#define ALLOCTOOL_CMD_H

#include "oink_cmd.h"
#include "oink_file.h"          // File

class AllocToolCmd : public virtual OinkCmd {
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

  // string to use to call free()
  char *free_func;
  // string to use to call xmalloc()
  char *xmalloc_func;
  // string to use to call verify()
  char *verify_func;

  AllocToolCmd();

  void readOneArg(int &argc, char **&argv);
  void dump();
  void printHelp();
  void initializeFromFlags();
};

#endif // ALLOCTOOL_CMD_H
