// see License.txt for copyright and terms of use

// Process command-line arguments.

#ifndef BULLET_CMD_H
#define BULLET_CMD_H

#include "oink_cmd.h"
#include "oink_file.h"          // File

class BulletCmd : public virtual OinkCmd {
  public:
//   bool ben_flag;                // demonstrate a flag for Ben
//   char *ben_string;             // demonstrate a string variable for Ben

  BulletCmd();

  void readOneArg(int &argc, char **&argv);
  void dump();
  void printHelp();
  void initializeFromFlags();
};

#endif // BULLET_CMD_H
