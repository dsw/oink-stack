// see License.txt for copyright and terms of use

#include "alloctool_cmd.h"      // this module
#include "oink_cmd_util.h"      // HANDLE_FLAG
#include "oink_util.h"
#include "oink_global.h"        // oinkCmd
#include <cstring>              // strdup
#include <cstdlib>              // atoi

AllocToolCmd::AllocToolCmd()
  : print_stack_alloc(false)
  , print_stack_alloc_addr_taken(false)
  , heapify_stack_alloc_addr_taken(false)
  , free_func("free")
  , xmalloc_func("xmalloc")
{}

void AllocToolCmd::readOneArg(int &argc, char **&argv) {
  int old_argc = argc;
  OinkCmd::readOneArg(argc, argv);
  if (old_argc != argc) return; // the superclass read one so we don't

  char *arg = argv[0];
  // please prefix the names of flags with arguments with '-a-'
  if (streq(arg, "-a-free-func")) {
    shift(argc, argv);
    free_func = strdup(shift(argc, argv)); // NOTE: use strdup!
    return;
  }
  else if (streq(arg, "-a-xmalloc-func")) {
    shift(argc, argv);
    xmalloc_func = strdup(shift(argc, argv)); // NOTE: use strdup!
    return;
  }
  // please prefix the names of boolean flags with '-fa-'
  HANDLE_FLAG(print_stack_alloc,
              "-fa-", "print-stack-alloc");
  HANDLE_FLAG(print_stack_alloc_addr_taken,
              "-fa-", "print-stack-alloc-addr-taken");
  HANDLE_FLAG(heapify_stack_alloc_addr_taken,
              "-fa-", "heapify-stack-alloc-addr-taken");
}

void AllocToolCmd::dump() {
  OinkCmd::dump();
  // alloctool arguments
  //
  // the idea here is to make the internal name be the same as the
  // external name with the dashes replaced by underscores
  printf("fa-print-stack-alloc: %s\n",
         boolToStr(print_stack_alloc));
  printf("fa-print-stack-alloc-addr-taken: %s\n",
         boolToStr(print_stack_alloc_addr_taken));
  printf("fa-heapify-stack-alloc-addr-taken: %s\n",
         boolToStr(heapify_stack_alloc_addr_taken));
  printf("a-free-func '%s'\n", free_func);
  printf("a-xmalloc-func '%s'\n", xmalloc_func);
}

void AllocToolCmd::printHelp() {
  OinkCmd::printHelp();
  printf
    (
     "\n"
     "alloctool flags that take an argument:\n"
     "  -a-free-func <value>     : set the name of the free function\n"
     "  -a-xmalloc-func <value>  : set the name of the xmalloc function\n"
     "\n"
     "alloctool boolean flags;\n"
     "    preceed by '-fa-' for positive sense,\n"
     "    by '-fa-no-' for negative sense.\n"
     "  -fa-print-stack-alloc            : print out every declaration\n"
     "    allocating a var on the stack\n"
     "  -fa-print-stack-alloc-addr-taken : print out every declaration\n"
     "    (1) allocating a var on the stack where\n"
     "    (2) the var also has its address taken\n"
     "  -fa-heapify-stack-alloc-addr-taken : heapify every declaration\n"
     "    (1) allocating a var on the stack where\n"
     "    (2) the var also has its address taken\n"
     "");
}

// push the state out to other places where it is needed; return value
// is true unless there is an error
void AllocToolCmd::initializeFromFlags() {
  OinkCmd::initializeFromFlags();

  if (instance_sensitive) {
    throw UserError(USER_ERROR_ExitCode,
                    "Can't use -fo-instance-sensitive with alloctool.");
  }

  if (print_stack_alloc +
      print_stack_alloc_addr_taken +
      heapify_stack_alloc_addr_taken > 2) {
    throw UserError
      (USER_ERROR_ExitCode,
       "Use at most one of:\n"
       "\t-fo-print-stack-alloc\n"
       "\t-fo-print-stack-alloc-addr-taken\n"
       "\t-fo-heapify-stack-alloc-addr-taken\n"
       );
  }

}
