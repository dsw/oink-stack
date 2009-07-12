// see License.txt for copyright and terms of use

#include "cfgprint_cmd.h"       // this module
#include "oink_cmd_util.h"      // HANDLE_FLAG
#include "oink_util.h"
#include <cstring>              // strdup
#include <cstdlib>              // atoi

CfgPrintCmd::CfgPrintCmd()
  : ben_flag  (false)
  , ben_string (NULL)
  , print_cfg (true)            // print the graph by default
{}

void CfgPrintCmd::readOneArg(int &argc, char **&argv) {
  int old_argc = argc;
  OinkCmd::readOneArg(argc, argv);
  if (old_argc != argc) return; // the superclass read one so we don't

  char *arg = argv[0];
  // please prefix the names of flags with arguments with '-c-'
  if (streq(arg, "-c-ben-string")) {
    shift(argc, argv);
    ben_string = strdup(shift(argc, argv)); // NOTE: use strdup!
    return;
  }
  // please prefix the names of boolean flags with '-fc-'
  HANDLE_FLAG(ben_flag, "-fc-", "ben-flag");
  HANDLE_FLAG(print_cfg, "-fc-", "print-cfg");
}

void CfgPrintCmd::dump() {
  OinkCmd::dump();
  // cfgprint arguments
  //
  // the idea here is to make the internal name be the same as the
  // external name with the dashes replaced by underscores
  printf("fc-ben-flag: %s\n", boolToStr(ben_flag));
  printf("m-ben-string: %s\n", ben_string);
  printf("fc-print-cfg: %s\n", boolToStr(print_cfg));
}

void CfgPrintCmd::printHelp() {
  OinkCmd::printHelp();
  printf
    ("cfgprint flags that take an argument:\n"
     "  -c-ben-string <value>     : set Ben's string\n"
     "cfgprint boolean flags; preceed by '-fc-' for positive sense, by '-fc-no-' for negative sense.\n"
     "  ben-flag                  : set Ben's flag\n"
     "  print-cfg                 : print the cfg in 'dot' output format to standard out\n"
     "");
}

// push the state out to other places where it is needed; return value
// is true unless there is an error
void CfgPrintCmd::initializeFromFlags() {
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
