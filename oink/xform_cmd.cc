// see License.txt for copyright and terms of use

#include "xform_cmd.h"          // this module
#include "oink_cmd_util.h"      // HANDLE_FLAG
#include "oink_util.h"
#include "oink_global.h"        // oinkCmd
#include <cstring>              // strdup
#include <cstdlib>              // atoi

XformCmd::XformCmd()
  : print_stack_alloc(false)
  , print_stack_alloc_addr_taken(false)
  , heapify_stack_alloc_addr_taken(false)
  , verify_cross_module_params(false)
  , localize_heap_alloc(false)
  , jimmy(false)
  , free_func("free")
  , xmalloc_func("xmalloc")
  , verify_func("verify")
{}

void XformCmd::readOneArg(int &argc, char **&argv) {
  int old_argc = argc;
  OinkCmd::readOneArg(argc, argv);
  if (old_argc != argc) return; // the superclass read one so we don't

  char *arg = argv[0];
  // please prefix the names of flags with arguments with '-x-'
  if (streq(arg, "-x-free-func")) {
    shift(argc, argv);
    free_func = strdup(shift(argc, argv)); // NOTE: use strdup!
    return;
  }
  else if (streq(arg, "-x-xmalloc-func")) {
    shift(argc, argv);
    xmalloc_func = strdup(shift(argc, argv)); // NOTE: use strdup!
    return;
  }
  else if (streq(arg, "-x-verify-func")) {
    shift(argc, argv);
    verify_func = strdup(shift(argc, argv)); // NOTE: use strdup!
    return;
  }
  // please prefix the names of boolean flags with '-fx-'
  HANDLE_FLAG(print_stack_alloc,
              "-fx-", "print-stack-alloc");
  HANDLE_FLAG(print_stack_alloc_addr_taken,
              "-fx-", "print-stack-alloc-addr-taken");
  HANDLE_FLAG(heapify_stack_alloc_addr_taken,
              "-fx-", "heapify-stack-alloc-addr-taken");
  HANDLE_FLAG(verify_cross_module_params,
              "-fx-", "verify-cross-module-params");
  HANDLE_FLAG(localize_heap_alloc,
              "-fx-", "localize-heap-alloc");
  HANDLE_FLAG(jimmy,
              "-fx-", "jimmy");
}

void XformCmd::dump() {
  OinkCmd::dump();
  // xform arguments
  //
  // the idea here is to make the internal name be the same as the
  // external name with the dashes replaced by underscores
  printf("fa-print-stack-alloc: %s\n",
         boolToStr(print_stack_alloc));
  printf("fa-print-stack-alloc-addr-taken: %s\n",
         boolToStr(print_stack_alloc_addr_taken));
  printf("fa-heapify-stack-alloc-addr-taken: %s\n",
         boolToStr(heapify_stack_alloc_addr_taken));
  printf("fa-verify-cross-module-params: %s\n",
         boolToStr(verify_cross_module_params));
  printf("fa-localize-heap-alloc: %s\n",
         boolToStr(localize_heap_alloc));
  printf("fa-jimmy: %s\n",
         boolToStr(jimmy));
  printf("a-free-func '%s'\n", free_func);
  printf("a-xmalloc-func '%s'\n", xmalloc_func);
  printf("a-verify-func '%s'\n", verify_func);
}

void XformCmd::printHelp() {
  OinkCmd::printHelp();
  printf
    (
     "\n"
     "xform flags that take an argument:\n"
     "  -x-free-func <value>    : set the name of the free function\n"
     "  -x-xmalloc-func <value> : set the name of the xmalloc function\n"
     "  -x-verify-func <value>  : set the name of the verify function\n"
     "\n"
     "xform boolean flags;\n"
     "    preceed by '-fx-' for positive sense,\n"
     "    by '-fx-no-' for negative sense.\n"
     "  -fx-print-stack-alloc            : print out every declaration\n"
     "    allocating a var on the stack\n"
     "  -fx-print-stack-alloc-addr-taken : print out every declaration\n"
     "    (1) allocating a var on the stack where\n"
     "    (2) the var also has its address taken\n"
     "  -fx-heapify-stack-alloc-addr-taken : heapify every declaration\n"
     "    (1) allocating a var on the stack where\n"
     "    (2) the var also has its address taken\n"
     "  -fx-verify-cross-module-params :\n"
     "    insert calls to verify the status of parameters that are pointers\n"
     "    to a class/struct/union type that is defined in this module\n"
     "  -fx-localize-heap-alloc :\n"
     "    localize calls to heap allocation calls: change calls to\n"
     "    malloc/free etc. so that they call class-local and module-local\n"
     "    malloc\n"
     "  -fx-jimmy :\n"
     "    move over rover and let jimmy take over\n"
     "");
}

// push the state out to other places where it is needed; return value
// is true unless there is an error
void XformCmd::initializeFromFlags() {
  OinkCmd::initializeFromFlags();

  if (instance_sensitive) {
    throw UserError(USER_ERROR_ExitCode,
                    "Can't use -fo-instance-sensitive with xform.");
  }

  if (print_stack_alloc +
      print_stack_alloc_addr_taken +
      heapify_stack_alloc_addr_taken +
      verify_cross_module_params +
      localize_heap_alloc +
      jimmy > 1) {
    throw UserError
      (USER_ERROR_ExitCode,
       "Use at most one of:\n"
       "\t-fx-print-stack-alloc\n"
       "\t-fx-print-stack-alloc-addr-taken\n"
       "\t-fx-heapify-stack-alloc-addr-taken\n"
       "\t-fx-verify-cross-module-params\n"
       "\t-fx-localize-heap-alloc\n"
       "\t-fx-jimmy\n"
       );
  }

}
