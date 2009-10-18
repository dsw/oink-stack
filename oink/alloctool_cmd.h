// see License.txt for copyright and terms of use

// Process command-line arguments.

#ifndef ALLOCTOOL_CMD_H
#define ALLOCTOOL_CMD_H

#include "oink_cmd.h"
#include "oink_file.h"          // File

class AllocToolCmd : public virtual OinkCmd {
  public:
//   bool ben_flag;                // demonstrate a flag for Ben
//   char *ben_string;             // demonstrate a string variable for Ben
//   bool print_ihg;               // print out the ihg in 'dot' output format to standard out
//   bool print_ast_histogram;     // print out how many of each ast kind

  AllocToolCmd();

  void readOneArg(int &argc, char **&argv);
  void dump();
  void printHelp();
  void initializeFromFlags();
};

#endif // ALLOCTOOL_CMD_H
