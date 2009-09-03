// see License.txt for copyright and terms of use

#include "staticprint_cmd.h"    // this module
#include "oink_cmd_util.h"      // HANDLE_FLAG
#include "oink_util.h"
#include <cstring>              // strdup
#include <cstdlib>              // atoi

StaticprintCmd::StaticprintCmd()
  : ben_flag (false)
  , ben_string (NULL)
  , print_ihg (false)
  , print_ast_histogram (false)
{}

void StaticprintCmd::readOneArg(int &argc, char **&argv) {
  int old_argc = argc;
  OinkCmd::readOneArg(argc, argv);
  if (old_argc != argc) return; // the superclass read one so we don't

  char *arg = argv[0];
  // please prefix the names of flags with arguments with '-s-'
  if (streq(arg, "-s-ben-string")) {
    shift(argc, argv);
    ben_string = strdup(shift(argc, argv)); // NOTE: use strdup!
    return;
  }
  // please prefix the names of boolean flags with '-fs-'
  HANDLE_FLAG(ben_flag, "-fs-", "ben-flag");
  HANDLE_FLAG(print_ihg, "-fs-", "print-ihg");
  HANDLE_FLAG(print_ast_histogram, "-fs-", "print-ast-histogram");
}

void StaticprintCmd::dump() {
  OinkCmd::dump();
  // staticprint arguments
  //
  // the idea here is to make the internal name be the same as the
  // external name with the dashes replaced by underscores
  printf("fs-ben-flag: %s\n", boolToStr(ben_flag));
  printf("s-ben-string: %s\n", ben_string);
  printf("fs-print-ihg: %s\n", boolToStr(print_ihg));
  printf("fs-print-ast-histogram: %s\n", boolToStr(print_ast_histogram));
}

void StaticprintCmd::printHelp() {
  OinkCmd::printHelp();
  printf
    ("staticprint flags that take an argument:\n"
     "  -s-ben-string <value>     : set Ben's string\n"
     "staticprint boolean flags; preceed by '-fs-' for positive sense, by '-fs-no-' for negative sense.\n"
     "  ben-flag                  : set Ben's flag\n"
     "  print-ihg                 : print the ihg in 'dot' output format to standard out\n"
     "  print-ast-histogram       : print a histogram of the AST nodes to standard out\n"
     "");
}

// push the state out to other places where it is needed; return value
// is true unless there is an error
void StaticprintCmd::initializeFromFlags() {
  OinkCmd::initializeFromFlags();

  // ben mode
  if (ben_flag) {
    // do some setup here for ben's flag
    printf("Hi Ben!\n");
  }

  // check here if there is some inconsistency between the flags
  if (ben_flag && ben_string) {
    throw UserError(USER_ERROR_ExitCode,
                    "It doesn't make sense to use the ben flag when the ben string is set.");
  }
}
