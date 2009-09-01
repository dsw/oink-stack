// see License.txt for copyright and terms of use

#include "bullet_cmd.h"    // this module
#include "oink_cmd_util.h"      // HANDLE_FLAG
#include "oink_util.h"
#include <cstring>              // strdup
#include <cstdlib>              // atoi

BulletCmd::BulletCmd()
//   : ben_flag (false)
//   , ben_string (NULL)
{}

void BulletCmd::readOneArg(int &argc, char **&argv) {
  int old_argc = argc;
  OinkCmd::readOneArg(argc, argv);
  if (old_argc != argc) return; // the superclass read one so we don't

  // off since currently unused
//   char *arg = argv[0];

  // please prefix the names of flags with arguments with '-b-'
//   if (streq(arg, "-b-ben-string")) {
//     shift(argc, argv);
//     ben_string = strdup(shift(argc, argv)); // NOTE: use strdup!
//     return;
//   }
  // please prefix the names of boolean flags with '-fb-'
//   HANDLE_FLAG(ben_flag, "-fb-", "ben-flag");
}

void BulletCmd::dump() {
  OinkCmd::dump();
  // bullet arguments
  //
  // the idea here is to make the internal name be the same as the
  // external name with the dashes replaced by underscores
//   printf("fb-ben-flag: %s\n", boolToStr(ben_flag));
//   printf("b-ben-string: %s\n", ben_string);
}

void BulletCmd::printHelp() {
  OinkCmd::printHelp();
  printf
    ("bullet flags that take an argument:\n"
//      "  -d-ben-string <value>     : set Ben's string\n"
//      "bullet boolean flags; preceed by '-fs-' for positive sense, by '-fs-no-' for negative sense.\n"
//      "  ben-flag                  : set Ben's flag\n"
     "");
}

// push the state out to other places where it is needed; return value
// is true unless there is an error
void BulletCmd::initializeFromFlags() {
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
