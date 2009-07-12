// see License.txt for copyright and terms of use

// Process command-line arguments.

#ifndef DFGPRINT_CMD_H
#define DFGPRINT_CMD_H

#include "oink_cmd.h"
#include "oink_file.h"          // File

class DfgPrintCmd : public virtual OinkCmd {
  public:
  bool ben_flag;                // demonstrate a flag for Ben
  char *ben_string;             // demonstrate a string variable for Ben
  bool print_dfg;               // print out the dfg in 'dot' output format to standard out

  DfgPrintCmd();

  void readOneArg(int &argc, char **&argv);
  void dump();
  void printHelp();
  void initializeFromFlags();
};

#endif // DFGPRINT_CMD_H
