// see License.txt for copyright and terms of use

// Processes command-line arguments for the oink empty-analysis
// proper.  Other tools inherit from it and re-use the command-line
// flags and their processing.

#ifndef OINK_CMD_H
#define OINK_CMD_H

#include "cc_lang.h"            // CCLang
#include "oink_file.h"
#include "strdict.h"
#include "strtable.h"
#include <string>

enum InputLang {
  UNKNOWN_InputLang,            // an unknown suffix

  KandR_C_InputLang,             // settings for K&R C
  ANSI_C89_InputLang,            // settings for ANSI C89
  ANSI_C99_InputLang,            // settings for ANSI C99
  GNU_C_InputLang,               // settings for GNU C
  GNU_KandR_C_InputLang,         // GNU 3.xx C + K&R compatibility
  GNU2_KandR_C_InputLang,        // GNU 2.xx C + K&R compatibility

  ANSI_Cplusplus_InputLang,      // settings for ANSI C++ 98
  GNU_Cplusplus_InputLang,       // settings for GNU C++

  SUFFIX_InputLang,             // Use the language appropriate to the suffix.
};

InputLang string2Lang(char const *langName);
InputLang getLangFromSuffix(char const *suff);
char *InputLang_toString(InputLang l);
void setLangState_fromInputLang(CCLang &lang, InputLang tgtLang);

class OinkCmd {
  public:
  char *theProgramName;         // unneeded, just stored for completeness

  // program files and positional arguments go into here
  ASTList<InputFile> inputFiles;
  ASTList<File> inputFilesFlat; // flattened into just a list of filenames
  std::string control; // control the cqual processing in a fine-grained way
  std::string func_filter; // filter out the listed Variables and their AST

  bool help;                    // print help message and exit
  bool verbose;                 // dump state before proceeding
  bool print_stages;            // print out each stage as we enter them
  bool exit_after_parse;        // exit after parsing
  bool exit_after_typecheck;    // exit after typechecking
  bool exit_after_elaborate;    // exit after elaborating

  bool module_print_class2mod;  // print the map from classes to modules
  bool print_func_attrs;        // print any function attributes
  bool func_gran;               // compute and print function granularity CFG
  bool func_gran_dot;           // print function granularity CFG in dot format
  bool func_gran_rev_mod_pub;   // further compute reverse graph, mod
                                // out by module equiv-class, and then
                                // print which functions are called
                                // from outside the module

  bool all_pass_filter;         // assert that all variables pass the filter
  bool print_startstop;         // bracket large output such as pretty-print with START/STOP
  bool print_ast;               // print the ast after parsing
  bool print_typed_ast;         // print the typed ast after typechecking
  bool print_elaborated_ast;    // print the AST after tcheck+elaboration
  bool print_ML_types;          // print types in ML-style
  bool print_buckets;           // print buckets (for instance sensitivity et al)
  bool print_stats;             // print analysis statistics
  bool print_sizes;             // print internal data structure sizes
  bool print_proc_stats;        // print process statistics
  bool pretty_print;            // pretty print after checking if there were no inconsistencies

  bool trace_link;              // whether to trace linking
  bool report_link_errors;      // report unsatisfied symbols
  bool report_unused_controls;  // report unused controls
  bool report_colorings;        // report module colorings as they are annotated onto sites
  bool print_controls_and_exit; // print out the controls and stop after they are read

  bool do_overload;             // do overload resolution; overrides default language setting
  bool do_op_overload;          // do operator overload resolution; as above
  bool do_elaboration;          // do elaboration

  bool check_AST_integrity;     // check the AST is a tree

  // flags for excluding unnecessary and strange syntax
  bool exclude_extra_star_amp;  // exclude consecutive '&*' or '&*' expressions

  // optimization: merge the Values for E_variable expressions and Variables
  bool merge_E_variable_and_var_values;

  bool instance_sensitive;      // turn on instance-sensitive dataflow
  bool array_index_flows;       // array index flows through array deref

  InputLang lang;               // which language to parse

  std::string srz;              // serialization output file

  OinkCmd();
  virtual ~OinkCmd(){}

  void appendInputFile(int &argc, char **&argv);
  void flattenInputFiles();

  virtual void readEnvironment(); // read environment variables
  virtual void readCommandLine(int &argc, char **&argv);
  virtual void readOneArg(int &argc, char **&argv);
  virtual void dump();
  virtual void printHelp();
  virtual void initializeFromFlags();

  // map the filenames listed in 'modFile' to 'module' into
  // file2module
  virtual void loadModule(StringRef modFile, StringRef module);
};

char const *boolToStr(bool b);
char *shift(int &argc, char **&argv, const char *errMsg = NULL);
bool endsWith(char const *a, char const *suffix);

#endif // OINK_CMD_H
