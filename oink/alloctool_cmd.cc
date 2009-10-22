// see License.txt for copyright and terms of use

#include "alloctool_cmd.h"    // this module
#include "oink_cmd_util.h"      // HANDLE_FLAG
#include "oink_util.h"
#include <cstring>              // strdup
#include <cstdlib>              // atoi

AllocToolCmd::AllocToolCmd()
  : print_stack_alloc_addr_taken(false)
{}

void AllocToolCmd::readOneArg(int &argc, char **&argv) {
  int old_argc = argc;
  OinkCmd::readOneArg(argc, argv);
  if (old_argc != argc) return; // the superclass read one so we don't

  char *arg = argv[0];
  // please prefix the names of flags with arguments with '-a-'
//   if (streq(arg, "-a-ben-string")) {
//     shift(argc, argv);
//     ben_string = strdup(shift(argc, argv)); // NOTE: use strdup!
//     return;
//   }
  // please prefix the names of boolean flags with '-fa-'
  HANDLE_FLAG(print_stack_alloc_addr_taken,
              "-fa-", "print-stack-alloc-addr-taken");
}

void AllocToolCmd::dump() {
  OinkCmd::dump();
  // alloctool arguments
  //
  // the idea here is to make the internal name be the same as the
  // external name with the dashes replaced by underscores
  printf("fa-print-stack-alloc-addr-taken: %s\n",
         boolToStr(print_stack_alloc_addr_taken));
//   printf("a-ben-string: %s\n", ben_string);
}

void AllocToolCmd::printHelp() {
  OinkCmd::printHelp();
  printf
    (
     "\n"
//      "alloctool flags that take an argument:\n"
//      "  -a-ben-string <value>     : set Ben's string\n"
     "\n"
     "alloctool boolean flags;\n"
     "    preceed by '-fa-' for positive sense,\n"
     "    by '-fa-no-' for negative sense.\n"
     "  -fa-print-stack-alloc-addr-taken : print out every declaration\n"
     "    (1) allocating a var on the stack where\n"
     "    (2) the var also has its address taken\n"
     "");
}

// push the state out to other places where it is needed; return value
// is true unless there is an error
void AllocToolCmd::initializeFromFlags() {
  OinkCmd::initializeFromFlags();

//   // ben mode
//   if (ben_flag) {
//     // do some setup here for ben's flag
//     printf("Hi Ben!\n");
//   }

//   // check here if there is some inconsistency between the flags
//   if (ben_flag && ben_string) {
//     throw UserError(USER_ERROR_ExitCode,
//                     "It doesn't make sense to use the ben flag when the ben string is set.");
//   }
}
