// see License.txt for copyright and terms of use

#include "dfgprint_cmd.h"       // this module
#include "oink_cmd_util.h"      // HANDLE_FLAG
#include "oink_util.h"
#include <cstring>             // strdup
#include <cstdlib>             // atoi

DfgPrintCmd::DfgPrintCmd()
  : ben_flag  (false)
  , ben_string (NULL)
  , print_dfg (true)            // print the graph by default
{}

void DfgPrintCmd::readOneArg(int &argc, char **&argv) {
  int old_argc = argc;
  OinkCmd::readOneArg(argc, argv);
  if (old_argc != argc) return; // the superclass read one so we don't

  char *arg = argv[0];
  // please prefix the names of flags with arguments with '-d-'
  if (streq(arg, "-d-ben-string")) {
    shift(argc, argv);
    ben_string = strdup(shift(argc, argv)); // NOTE: use strdup!
    return;
  }
  // please prefix the names of boolean flags with '-fd-'
  HANDLE_FLAG(ben_flag, "-fd-", "ben-flag");
  HANDLE_FLAG(print_dfg, "-fd-", "print-dfg");
}

void DfgPrintCmd::dump() {
  OinkCmd::dump();
  // dfgprint arguments
  //
  // the idea here is to make the internal name be the same as the
  // external name with the dashes replaced by underscores
  printf("fd-ben-flag: %s\n", boolToStr(ben_flag));
  printf("m-ben-string: %s\n", ben_string);
  printf("fd-print-dfg: %s\n", boolToStr(print_dfg));
}

void DfgPrintCmd::printHelp() {
  OinkCmd::printHelp();
  printf
    ("dfgprint flags that take an argument:\n"
     "  -d-ben-string <value>     : set Ben's string\n"
     "dfgprint boolean flags; preceed by '-fd-' for positive sense, by '-fd-no-' for negative sense.\n"
     "  ben-flag                  : set Ben's flag\n"
     "  print-dfg                 : print the dfg in 'dot' output format to standard out\n"
     "");
}

// push the state out to other places where it is needed; return value
// is true unless there is an error
void DfgPrintCmd::initializeFromFlags() {
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
