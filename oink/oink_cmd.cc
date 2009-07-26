// see License.txt for copyright and terms of use

#include "oink_cmd.h"           // this module
#include "oink_global.h"        // globalStrTable
#include "oink_cmd_util.h"      // HANDLE_FLAG
#include "cc_type.h"
#include "trace.h"              // tracingSys, traceAddSys
#include "oink_util.h"
#include <cstring>
#include <fstream>              // ofstream, ifstream

InputLang string2Lang(char const *langName) {
  if (0) {}                     // orthogonality
  else if (streq(langName, "KandR_C"))        return KandR_C_InputLang;
  else if (streq(langName, "ANSI_C89"))       return ANSI_C89_InputLang;
  else if (streq(langName, "ANSI_C99"))       return ANSI_C99_InputLang;
  else if (streq(langName, "GNU_C"))          return GNU_C_InputLang;
  else if (streq(langName, "GNU_KandR_C"))    return GNU_KandR_C_InputLang;
  else if (streq(langName, "GNU2_KandR_C"))   return GNU2_KandR_C_InputLang;
  else if (streq(langName, "ANSI_Cplusplus")) return ANSI_Cplusplus_InputLang;
  else if (streq(langName, "GNU_Cplusplus"))  return GNU_Cplusplus_InputLang;
  else if (streq(langName, "SUFFIX"))         return SUFFIX_InputLang;
  else {
    throw UserError
      (USER_ERROR_ExitCode,
       "Illegal argument to -o-lang.  Legal arguments are:\n"
       "\tKandR_C, ANSI_C89, ANSI_C99, GNU_C, GNU_KandR_C, GNU2_KandR_C,\n"
       "\tANSI_Cplusplus, GNU_Cplusplus, SUFFIX.");
  }
}

InputLang getLangFromSuffix(char const *suff) {
  if (streq(suff, ".i") ||
      streq(suff, ".c")) {
    return GNU_KandR_C_InputLang;
  } else if (streq(suff, ".ii") ||
             streq(suff, ".cc") ||
             streq(suff, ".cpp") ||
             streq(suff, ".cxx") ||
             streq(suff, ".c++") ||
             streq(suff, ".C")
             ) {
    return GNU_Cplusplus_InputLang;
  } else {
    return UNKNOWN_InputLang;
  }
}

char *InputLang_toString(InputLang l) {
  switch(l) {
  default: xfailure("illegal InputLang");
  case KandR_C_InputLang:        return "KandR_C";
  case ANSI_C89_InputLang:       return "ANSI_C89";
  case ANSI_C99_InputLang:       return "ANSI_C99";
  case GNU_C_InputLang:          return "GNU_C";
  case GNU_KandR_C_InputLang:    return "GNU_KandR_C";
  case GNU2_KandR_C_InputLang:   return "GNU2_KandR_C";
  case ANSI_Cplusplus_InputLang: return "ANSI_Cplusplus";
  case GNU_Cplusplus_InputLang:  return "GNU_Cplusplus";
  case SUFFIX_InputLang:         return "SUFFIX";
  }
}

void setLangState_fromInputLang(CCLang &lang, InputLang tgtLang) {
  switch(tgtLang) {
  default: xfailure("illegal InputLang");
  case KandR_C_InputLang: lang.KandR_C(); break;
  case ANSI_C89_InputLang: lang.ANSI_C89(); break;
  case ANSI_C99_InputLang: lang.ANSI_C99(); break;
  case GNU_C_InputLang: lang.GNU_C(); break;
  case GNU_KandR_C_InputLang: lang.GNU_KandR_C(); break;
  case GNU2_KandR_C_InputLang: lang.GNU2_KandR_C(); break;
  case ANSI_Cplusplus_InputLang: lang.ANSI_Cplusplus(); break;
  case GNU_Cplusplus_InputLang: lang.GNU_Cplusplus(); break;
  case SUFFIX_InputLang:
    xfailure("setInputLang() shouldn't get SUFFIX_InputLang");
    break;
  }
  globalLang.enableAllWarnings();
}

// ****

char const *boolToStr(bool b) {return b?"true":"false";}

// like perl shift
char *shift(int &argc, char **&argv, const char *errMsg) {
  if (!argc || !argv[0] || !argv[0][0]) {
    if (!errMsg) {
      errMsg = "Command line ends with a flag that needs an argument";
    }
    throw UserError(USER_ERROR_ExitCode, errMsg);
  }
  char *ret = argv[0];
  ++argv;
  --argc;
  return ret;
}

bool endsWith(char const *a, char const *suffix) {
  size_t len_a = strlen(a);
  size_t len_suffix = strlen(suffix);

  if (len_suffix > len_a) return false;

  return 0 == memcmp(a + len_a - len_suffix, suffix, len_suffix+1);
}

// ****

OinkCmd::OinkCmd()
  : theProgramName       (NULL)

  , control              ()
  , func_filter          ()

  , help                 (false)
  , verbose              (false)
  , print_stages         (false)
  , exit_after_parse     (false)
  , exit_after_typecheck (false)
  , exit_after_elaborate (false)

  , func_gran            (false)
  , func_gran_dot        (false)
  , all_pass_filter      (false)
  , print_startstop      (true)
  , print_ast            (false)
  , print_typed_ast      (false)
  , print_elaborated_ast (false)
  , print_ML_types       (false)
  , print_buckets        (false)
  , print_stats          (false)
  , print_sizes          (false)
  , print_proc_stats     (false)
  , pretty_print         (false)

  , trace_link                 (false)
  , report_link_errors         (false)
  , report_unused_controls     (false)
  , report_colorings           (false)
  , print_controls_and_exit    (false)

  , do_overload          (true)
  , do_op_overload       (true)
  , do_elaboration       (true)

  , check_AST_integrity  (true)
  , exclude_extra_star_amp(false)

  , merge_E_variable_and_var_values(true)

  , instance_sensitive   (false)
  , array_index_flows    (false)

  , lang                 (SUFFIX_InputLang)
  , srz                  ()

{}

void OinkCmd::appendInputFile(int &argc, char **&argv) {
  char *name = shift(argc, argv);
  xassert(name);
  if (name[0]=='-') {
    throw UserError(USER_ERROR_ExitCode,
                    stringc << "Unrecognized flag '" << name << "'.  Please try --help.");
  }
  inputFiles.append(new File(name));
}

void OinkCmd::flattenInputFiles() {
  xassert(inputFilesFlat.isEmpty());
  FOREACH_ASTLIST(InputFile, inputFiles, iter) {
    iter.data()->appendSelfIntoList(inputFilesFlat);
  }
}

// read environment variables
void OinkCmd::readEnvironment()
{
}

// extracts all the flags; updates argc and argv to contain only the
// positional parameters
void OinkCmd::readCommandLine(int &argc, char **&argv) {
  theProgramName = shift(argc, argv);
  while(argv[0]) {
    int old_argc = argc;
    readOneArg(argc, argv);
    if (old_argc == argc) {
      appendInputFile(argc, argv);
    }
  }
}

void OinkCmd::readOneArg(int &argc, char **&argv) {
  char *arg = argv[0];
  xassert(arg);

  if (streq(arg, "-tr")) {
    shift(argc, argv);
    // so you can't comma-separate them as you can in elsa
    traceAddSys(shift(argc, argv));
    return;
  }
  if (streq(arg, "-o-program-files")) {
    shift(argc, argv);
    inputFiles.append(new ProgramFile(globalStrTable(shift(argc, argv))));
    return;
  }
  if (streq(arg, "-o-control")) {
    shift(argc, argv);
    control = shift(argc, argv);
    return;
  }
  if (streq(arg, "-o-func-filter")) {
    shift(argc, argv);
    func_filter = shift(argc, argv);
    return;
  }
  if (streq(arg, "-o-lang")) {
    shift(argc, argv);
    char const *langName = shift(argc, argv, "Missing argument to -o-lang");
    lang = string2Lang(langName);
    return;
  }
  if (streq(arg, "-o-module")) {
    shift(argc, argv);
    char const *arg0 = shift(argc, argv,  "Missing argument to -o-module");
    // load module
    StringRef module = globalStrTable(arg0);
    if (loadedModules.contains(module)) {
      throw UserError(USER_ERROR_ExitCode,
                      stringc << "Request to load module twice: " << module);
    }
    loadedModules.add(module);
    // must be new due to set check above; don't need prependUnique()
    moduleList.prepend(const_cast<char*>(module));
    loadModule(module);
    return;
  }
  if (streq(arg, "-o-module-default")) {
    shift(argc, argv);
    char *arg0 = shift(argc, argv, "Missing argument to -o-module-default");
    // set default module
    if (defaultModule) {
      throw UserError(USER_ERROR_ExitCode, "Default module already specified.");
    }
    defaultModule = globalStrTable(arg0);
    // not guaranteed to be new so I DO use prependUnique()
    moduleList.prependUnique(const_cast<char*>(defaultModule));
    cout << "defaultModule: " << defaultModule << endl;
    return;
  }

  // we allow a more usual help flag in addition to -fo-help
  if (streq(arg, "-help") || streq(arg, "--help")) {
    shift(argc, argv);
    help = true;
    return;
  }
  HANDLE_FLAG(help, "-fo-", "help");
  HANDLE_FLAG(verbose, "-fo-", "verbose");
  HANDLE_FLAG(print_stages, "-fo-", "print-stages");
  HANDLE_FLAG(exit_after_parse, "-fo-", "exit-after-parse");
  HANDLE_FLAG(exit_after_typecheck, "-fo-", "exit-after-typecheck");
  HANDLE_FLAG(exit_after_elaborate, "-fo-", "exit-after-elaborate");

  HANDLE_FLAG(func_gran, "-fo-", "func-gran");
  HANDLE_FLAG(func_gran_dot, "-fo-", "func-gran-dot");
  HANDLE_FLAG(all_pass_filter, "-fo-", "all-pass-filter");
  HANDLE_FLAG(print_startstop, "-fo-", "print-startstop");
  HANDLE_FLAG(print_ast, "-fo-", "print-ast");
  HANDLE_FLAG(print_typed_ast, "-fo-", "print-typed-ast");
  HANDLE_FLAG(print_elaborated_ast, "-fo-", "print-elaborated-ast");
  HANDLE_FLAG(print_ML_types, "-fo-", "print-ML-types");
  HANDLE_FLAG(print_buckets, "-fo-", "print-buckets");
  HANDLE_FLAG(print_stats, "-fo-", "print-stats");
  HANDLE_FLAG(print_sizes, "-fo-", "print-sizes");
  HANDLE_FLAG(print_proc_stats, "-fo-", "print-proc-stats");
  HANDLE_FLAG(pretty_print, "-fo-", "pretty-print");

  HANDLE_FLAG(trace_link, "-fo-", "trace-link");
  HANDLE_FLAG(report_link_errors, "-fo-", "report-link-errors");
  // HANDLE_FLAG(report_link_errors, "-fo-", "report-unsatisfied-symbols"); // old name
  HANDLE_FLAG(report_unused_controls, "-fo-", "report-unused-controls");
  HANDLE_FLAG(report_colorings, "-fo-", "report-colorings");
  HANDLE_FLAG(print_controls_and_exit, "-fo-", "print-controls-and-exit");

  HANDLE_FLAG(do_overload, "-fo-", "do-overload");
  HANDLE_FLAG(do_op_overload, "-fo-", "do-op-overload");
  HANDLE_FLAG(do_elaboration, "-fo-", "do-elaboration");

  HANDLE_FLAG(check_AST_integrity, "-fo-", "check-AST-integrity");
  HANDLE_FLAG(exclude_extra_star_amp, "-fo-", "exclude-extra-star-amp");

  HANDLE_FLAG(merge_E_variable_and_var_values, "-fo-", "merge-E_variable-and-var-values");

  HANDLE_FLAG(instance_sensitive, "-fo-", "instance-sensitive");
  HANDLE_FLAG(array_index_flows, "-fo-", "array-index-flows");

  if (streq(arg, "-o-srz")) {
    shift(argc, argv);
    if (!srz.empty()) {
      throw UserError(USER_ERROR_ExitCode, "can specify at most one serialization output file");
    }
    srz = strdup(shift(argc, argv));
    return;
  }
}

void OinkCmd::dump() { // for -fo-verbose
  printf("--- compile-time options\n");

  printf("gcc version: %s\n", __VERSION__);

  printf("debugging:");
#ifdef NDEBUG
  printf(" NDEBUG");
#endif
#ifdef USE_SERIAL_NUMBERS
  printf(" USE_SERIAL_NUMBERS");
#endif
  printf("\n");

  printf("archiving:");
#ifdef ARCHIVE_SRZ_DIR
  printf(" ARCHIVE_SRZ_DIR");
#endif
#ifdef ARCHIVE_SRZ_ZIP
  printf(" ARCHIVE_SRZ_ZIP");
#endif

  // FIX: this is really a property of the ast package, which could
  // have been built with different flags; in fact, right now it is
  // just defined there
// #ifdef CANONICAL_XML_IDS
//   printf(" CANONICAL_XML_IDS");
// #endif

  printf("\n");
  printf("\n");

  printf("--- run-time options\n");
//    printf("theProgramName '%s'\n", theProgramName);

  printf("o-lang: %s\n", InputLang_toString(lang));

  printf("o-inputFiles:\n");
  FOREACH_ASTLIST(InputFile, inputFiles, iter) {
    iter.data()->dump();
  }
  printf("o-inputFiles (flattened):\n");
  FOREACH_ASTLIST(File, inputFilesFlat, iter) {
    iter.data()->dump();
  }
  printf("o-control: %s\n", control.c_str());
  printf("o-func-filter: %s\n", func_filter.c_str());
  printf("o-srz: %s\n", srz.c_str());

  printf("fo-help: %s\n", boolToStr(help));
  printf("fo-verbose: %s\n", boolToStr(verbose));
  printf("fo-print-stages: %s\n", boolToStr(print_stages));
  printf("fo-exit-after-parse: %s\n", boolToStr(exit_after_parse));
  printf("fo-exit-after-typecheck: %s\n", boolToStr(exit_after_typecheck));
  printf("fo-exit-after-elaborate: %s\n", boolToStr(exit_after_elaborate));

  printf("fo-func-gran: %s\n", boolToStr(func_gran));
  printf("fo-func-gran-dot: %s\n", boolToStr(func_gran_dot));
  printf("fo-all-pass-filter: %s\n", boolToStr(all_pass_filter));
  printf("fo-print-startstop: %s\n", boolToStr(print_startstop));
  printf("fo-print-ast: %s\n", boolToStr(print_ast));
  printf("fo-print-typed-ast: %s\n", boolToStr(print_typed_ast));
  printf("fo-print-elaborated-ast: %s\n", boolToStr(print_elaborated_ast));
  printf("fo-print-ML-types: %s\n", boolToStr(print_ML_types));
  printf("fo-print-buckets: %s\n", boolToStr(print_buckets));
  printf("fo-print-stats: %s\n", boolToStr(print_stats));
  printf("fo-print-sizes: %s\n", boolToStr(print_sizes));
  printf("fo-print-proc-stats: %s\n", boolToStr(print_proc_stats));
  printf("fo-pretty-print: %s\n", boolToStr(pretty_print));

  printf("fo-trace-link: %s\n", boolToStr(trace_link));
  printf("fo-report-link-errors: %s\n", boolToStr(report_link_errors));
  printf("fo-report-unused-controls: %s\n", boolToStr(report_unused_controls));
  printf("fo-report-colorings: %s\n", boolToStr(report_colorings));
  printf("fo-print-controls-and-exit: %s\n", boolToStr(print_controls_and_exit));

  printf("fo-do-overload: %s\n", boolToStr(do_overload));
  printf("fo-do-op-overload: %s\n", boolToStr(do_op_overload));
  printf("fo-do-elaboration: %s\n", boolToStr(do_elaboration));

  printf("fo-check-AST-integrity: %s\n", boolToStr(check_AST_integrity));
  printf("fo-exclude-extra-star-amp: %s\n", boolToStr(exclude_extra_star_amp));

  printf("fo-merge-E_variable-and-var-values: %s\n", boolToStr(merge_E_variable_and_var_values));

  printf("fo-instance-sensitive: %s\n", boolToStr(instance_sensitive));
  printf("fo-array-index-flows: %s\n", boolToStr(array_index_flows));
}

void OinkCmd::printHelp() {
  printf
    ("%s",
     "All arguments not starting with a '-' are considered to be input files.\n"
     "\n"
     "oink flags that take an argument:\n"                                            ///
     "  -o-lang LANG             : specify the input language; one of:\n"
     "        KandR_C, ANSI_C89, ANSI_C99, GNU_C, GNU_KandR_C, GNU2_KandR_C,\n"
     "        ANSI_Cplusplus, GNU_Cplusplus, SUFFIX.\n"
     "  -o-program-files FILE    : add *contents* of FILE to list of input files\n"
     "  -o-control FILE          : give a file for controlling the behavior of oink\n"
     "  -o-func-filter FILE      : give a file listing Variables to be filtered out\n"
     "  -o-srz FILE              : serialize to FILE\n"
     "\n"
     "oink boolean flags; precede by '-fo-no-' for the negative sense.\n"
     "  -fo-help, -help, --help  : print this message and exit\n"
     "  -fo-verbose              : print the setting of each flag\n"
     "  -fo-print-stages         : announce each processing stage\n"
     "  -fo-exit-after-parse     : exit after parsing\n"
     "  -fo-exit-after-typecheck : exit after typechecking\n"
     "  -fo-exit-after-elaborate : exit after elaborating\n"
     "  -fo-print-startstop      : delimit transformed output with cut lines\n"
     "\n"
     "  -fo-func-gran            : compute and print function granularity CFG only\n"
     "                                (use -o-srz to write to file)\n"
     "  -fo-func-gran-dot        : print function granularity CFG in dot format\n"
     "  -fo-all-pass-filter      : assert that all variables pass the filter\n"
     "  -fo-print-ast            : print the ast\n"
     "  -fo-print-typed-ast      : print the ast after typechecking\n"
     "  -fo-print-elaborated-ast : print the ast after elaboration\n"
     "  -fo-print-ML-types       : print types in ML-style; AST print, not pretty-pr\n"
     "  -fo-print-buckets        : print buckets\n"
     "  -fo-print-stats          : print analysis stats\n"
     "  -fo-print-sizes          : print internal data structure sizes and exit\n"
     "  -fo-print-proc-stats     : print process stats\n"
     "  -fo-pretty-print         : print the ast as source\n"
     "\n"
     "  -fo-trace-link           : trace linking\n"
     "  -fo-report-link-errors   : print un-satisfied/over-satisfied function symbols in linker\n"
     "  -fo-report-unused-controls  : print controls that go unused\n"
     "  -fo-report-colorings        : report module colorings\n"
     "  -fo-print-controls-and-exit : print the controls and stop\n"
     "\n"
     "  -fo-do-overload          : do overload res., overriding language default\n"
     "  -fo-do-op-overload       : do op overload res., overriding language default\n"
     "  -fo-do-elaboration       : do elaboration (for C++)\n"
     "\n"
     "  -fo-check-AST-integrity  : check the AST is a tree\n"
     "  -fo-exclude-extra-star-amp : exclude consecutive '&*' or '&*' exprs\n"
     "\n"
     "  -fo-merge-E_variable-and-var-values :\n"
     "          optimization: merge Values for E_variable expressions and Variables\n"
     "\n"
     "  -fo-instance-sensitive   : C mode only.\n"
     "                             1) fields get unique values per struct instance\n"
     "                             2) 'void's get unique values by type\n"
     "  -fo-array-index-flows    : the array index flows through the array deref"
     "");
}

void OinkCmd::initializeFromFlags() {
  if (help) {
    printHelp();
    throw UserError(NORMAL_ExitCode);
  }
  if (pretty_print && !print_startstop && inputFilesFlat.count()>1) {
    throw UserError
      (USER_ERROR_ExitCode,
       "You may not use -fo-pretty-print and -fo-no-print-startstop when "
       "there is more than one input file.");
  }
  if (print_ML_types) {
    Type::printAsML = true;
  }

  // Initialize anything from tracing flags.  This is just to imitate
  // the functionality of elsa/ccparse; otherwise I could just have my
  // own flags for these.
  if (tracingSys("nohashline")) {
    sourceLocManager->useHashLines = false;
  }

  // quarl: default useOriginalOffset to off.  Otherwise we if the original
  // (pre-cpp) file doesn't exist, we die with an obtuse error message (this
  // should be fixed in smbase/srcloc...)

  sourceLocManager->useOriginalOffset = false;
  if (tracingSys("no-orig-offset")) {
    sourceLocManager->useOriginalOffset = false;
  }

  if (tracingSys("orig-offset")) {
    sourceLocManager->useOriginalOffset = false;
  }

  variablesLinkerVisibleEvenIfNonStaticDataMember = !oinkCmd->instance_sensitive;

  // FIX: Can't do this since the lang can change to C++ depending on
  // the suffix; there are assertions elsewhere.
//    if (instance_sensitive && lang.isCplusplus) {
//      throw UserError(USER_ERROR_ExitCode,
//                      "It is not yet implemented to use -fo-instance-sensitive in C++.");
//    }

  if (all_pass_filter && oinkCmd->func_filter.empty()) {
    throw UserError
      (USER_ERROR_ExitCode, "If you use -fo-all-pass-filter you must also provide a filter.");
  }
}

void OinkCmd::loadModule(StringRef module) {
  cout << "loading module " << module << endl;
  stringBuilder moduleFile(module);
  moduleFile << ".mod";

  std::ifstream moduleIn(moduleFile);
  if (!moduleIn) {
    throw UserError(USER_ERROR_ExitCode, stringc << "Cannot read module file " << moduleFile);
  }

  while(moduleIn) {
    string line;
    getline(moduleIn, line);
    // comments: delete everything after the hash
    char const *hashPos = strstr(line.c_str(), "#");
    if (hashPos) line = line.substring(0, hashPos - line.c_str());
    // trim the line
    line = trimWhitespace(line);
    // skip blank lines
    if (line.empty()) continue;
    // add the file named by the line to the module map
    cout << "\tadding to modules map " << line << "->" << module << endl;
    file2module.add(strdup(line.c_str()), module);
  }
}
