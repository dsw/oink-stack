// see License.txt for copyright and terms of use

// Process command-line arguments.

#ifndef CFGPRINT_CMD_H
#define CFGPRINT_CMD_H

#include "oink_cmd.h"
#include "oink_file.h"          // File

class CfgPrintCmd : public virtual OinkCmd {
  public:
  bool ben_flag;                // demonstrate a flag for Ben
  char *ben_string;             // demonstrate a string variable for Ben
  bool print_cfg;               // print out the cfg in 'dot' output format to standard out

  CfgPrintCmd();

  void readOneArg(int &argc, char **&argv);
  void dump();
  void printHelp();
  void initializeFromFlags();
};

#endif // CFGPRINT_CMD_H
