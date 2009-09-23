// see License.txt for copyright and terms of use

#include "qual_cmd.h"
#include "qual_libqual_iface.h" // LibQual::
#include "oink_cmd_util.h"      // HANDLE_FLAG
#include "oink_util.h"
#include "oink_global.h"        // globalLang
#include "libc_missing.h"       // strndup0
#include <cstdlib>              // atoi
#include <cstring>              // strdup

QSpec::QSpec(char const *original0, char *qual0, int depth0)
  : original(original0)
  , qual(qual0)
  , depth(depth0)
{
  xassert(original);
  xassert(qual);
  xassert(depth >= 0);
}

void QSpec::dump() {
  printf("qual '%s', depth '%d'\n", qual, depth);
}

// turn a string of the form '$tainted**' into a spec
static QSpec *parseQSpecString(char const * const specStr) {
  xassert(specStr);
  if (index(specStr, ' ')) {
    throw UserError(USER_ERROR_ExitCode, "Do not put spaces into a qualifier specifier string.");
  }
  // find the base qual
  char *qual = NULL;
  char *star = NULL;
  char const *specStr0 = specStr;
  star = index(specStr0, '*');
  if (star) {
    qual = strndup0(specStr0, star-specStr0);
  } else {
    qual = strdup(specStr0);
  }
  // find the depth
  int depth = 0;
  while((star = index(specStr0, '*'))) {
    ++depth;
    specStr0 = star + 1;
  }
  // make the result
  return new QSpec(specStr, qual, depth);
}

QualCmd::QualCmd()
  : config                   (NULL)
  , hotspots                 (0)
  , print_quals_graph        (false)
  , strict_const             (false)
  , casts_preserve           (false)
  , use_const_subtyping      (true) // default is "on" says the manual
  , flow_sensitive           (false)

  // in cqual the default is "off" from empirical observation, not the
  // manual, but the graph it generates is deceptive when off, so I
  // turn it on by default
  , ugly                     (true)

  , poly                     (false)

  // Mozilla-specific
  , reg_stack_regfunc         (NULL)
  , reg_stack_un_regfunc      (NULL)
  , reg_stack_array_regfunc   (NULL)
  , reg_stack_array_un_regfunc(NULL)

  , inference                (true)
  , print_trans_qual         (false)
  , name_expressions         (true)
  , name_vars                (true)
  , name_if_missing          (true)
  , casts_preserve_below_functions(true) // Jeff says the default is to not do this; Revolution!

  // structural flow
  , flow_compoundUp          (false)
  , flow_compoundDown        (false)
  , flow_pointUp             (false)
  , flow_pointDown           (false)
  , flow_refUp               (false)
  , flow_refDown             (false)

  , stackness                (false)

  , explain_errors           (true)
  , max_errors               (25)
  , ret_inf                  (true)
  , name_with_loc            (false)
  , name_with_serialno       (false)

  , merge_ref_ptr_qvars      (true)

  , module_access            (false)
  , module_write             (false)
  , module_trust             (false)
  , module_print_class2mod   (false)

  , compactify_graph         (CG_IFSRZ)
{}

void QualCmd::readEnvironment()
{
  OinkCmd::readEnvironment();
  const char *env_config = getenv("QUALCC_CONFIG");
  if (env_config && *env_config)
    config = strdup(env_config);
}

void QualCmd::readOneArg(int &argc, char **&argv) {
  int old_argc = argc;
  OinkCmd::readOneArg(argc, argv);
  if (old_argc != argc) return; // the superclass read one so we don't

  char *arg = argv[0];
  if (streq(arg, "-q-config")) {
    shift(argc, argv);
    config = shift(argc, argv);
    return;
  }
  if (streq(arg, "-q-hotspots")) {
    shift(argc, argv);
    hotspots = atoi(shift(argc, argv));
    return;
  }

  HANDLE_FLAG(print_quals_graph, "-fq-", "print-quals-graph");
  HANDLE_FLAG(strict_const, "-fq-", "strict-const");
  HANDLE_FLAG(casts_preserve, "-fq-", "casts-preserve");
  HANDLE_FLAG(use_const_subtyping, "-fq-", "use-const-subtyping");
  HANDLE_FLAG(ugly, "-fq-", "ugly");
  HANDLE_FLAG(poly, "-fq-", "poly");

  if (streq(arg, "-q-catch-qual")) {
    shift(argc, argv);
    catch_qual.append(strdup(shift(argc, argv)));
    return;
  }
  if (streq(arg, "-q-exclude-global")) {
    shift(argc, argv);
    exclude_global.append(strdup(shift(argc, argv)));
    return;
  }
  if (streq(arg, "-q-exclude-cast")) {
    shift(argc, argv);
    exclude_cast.append(strdup(shift(argc, argv)));
    return;
  }
  if (streq(arg, "-q-func-param-ret-qual")) {
    shift(argc, argv);
    func_param_ret_qual.append(strdup(shift(argc, argv)));
    return;
  }

  if (streq(arg, "-q-reg-stack-regfunc")) {
    shift(argc, argv);
    reg_stack_regfunc = strdup(shift(argc, argv));
    return;
  }
  if (streq(arg, "-q-reg-stack-un-regfunc")) {
    shift(argc, argv);
    reg_stack_un_regfunc = strdup(shift(argc, argv));
    return;
  }
  if (streq(arg, "-q-reg-stack-array-regfunc")) {
    shift(argc, argv);
    reg_stack_array_regfunc = strdup(shift(argc, argv));
    return;
  }
  if (streq(arg, "-q-reg-stack-array-un-regfunc")) {
    shift(argc, argv);
    reg_stack_array_un_regfunc = strdup(shift(argc, argv));
    return;
  }

  // these flags take a QSPEC argument, not just a qualifier
  if (streq(arg, "-q-const-when-param")) {
    shift(argc, argv);
    const_when_param.append(parseQSpecString(shift(argc, argv)));
    return;
  }
  if (streq(arg, "-q-reg-stack")) {
    shift(argc, argv);
    reg_stack.append(parseQSpecString(shift(argc, argv)));
    return;
  }
  if (streq(arg, "-q-exl-perm-alias")) {
    shift(argc, argv);
    exl_perm_alias.append(parseQSpecString(shift(argc, argv)));
    return;
  }
  if (streq(arg, "-q-exl-value-ref-call")) {
    shift(argc, argv);
    exl_value_ref_call.append(parseQSpecString(shift(argc, argv)));
    return;
  }
  if (streq(arg, "-q-arg-local-var")) {
    shift(argc, argv);
    arg_local_var.append(parseQSpecString(shift(argc, argv)));
    return;
  }
  if (streq(arg, "-q-temp-immediately-used")) {
    shift(argc, argv);
    temp_immediately_used.append(parseQSpecString(shift(argc, argv)));
    return;
  }

  HANDLE_FLAG(inference, "-fq-", "inference");
  HANDLE_FLAG(print_trans_qual, "-fq-", "print-trans-qual");
  HANDLE_FLAG(name_expressions, "-fq-", "name-expressions");
  HANDLE_FLAG(name_vars, "-fq-", "name-vars");
  HANDLE_FLAG(name_if_missing, "-fq-", "name-if-missing");

  if (streq(arg, "-fq-names")) {
    shift(argc, argv);
    name_expressions = true;
    name_vars = true;
    name_if_missing = true;
    return;
  }
  if (streq(arg, "-fq-no-names")) {
    shift(argc, argv);
    name_expressions = false;
    name_vars = false;
    name_if_missing = false;
    return;
  }

  HANDLE_FLAG(casts_preserve_below_functions, "-fq-",
              "casts-preserve-below-functions");

  HANDLE_FLAG(flow_compoundUp, "-fq-", "flow-compoundUp");
  HANDLE_FLAG(flow_compoundDown, "-fq-", "flow-compoundDown");
  HANDLE_FLAG(flow_pointUp, "-fq-", "flow-pointUp");
  HANDLE_FLAG(flow_pointDown, "-fq-", "flow-pointDown");
  HANDLE_FLAG(flow_refUp, "-fq-", "flow-refUp");
  HANDLE_FLAG(flow_refDown, "-fq-", "flow-refDown");

  HANDLE_FLAG(stackness, "-fq-", "stackness");

  HANDLE_FLAG(explain_errors, "-fq-", "explain-errors");

  if (streq(arg, "-q-max-errors")) {
    shift(argc, argv);
    max_errors = atoi(shift(argc, argv));
    return;
  }

  HANDLE_FLAG(ret_inf, "-fq-", "ret-inf");
  HANDLE_FLAG(name_with_loc, "-fq-", "name-with-loc");
  HANDLE_FLAG(name_with_serialno, "-fq-", "name-with-serialno");

  HANDLE_FLAG(merge_ref_ptr_qvars, "-fq-", "merge-ref-ptr-qvars");

  // HANDLE_FLAG(compactify_graph, "-fq-", "compactify-graph");
  if (streq(arg, "-fq-compactify-graph") ||
      streq(arg, "-fq-compactify-graph=yes") ||
      streq(arg, "-fq-compactify-graph=ifsrz"))
  {
    shift(argc, argv);
    compactify_graph = CG_IFSRZ;
    return;
  }
  if (streq(arg, "-fq-no-compactify-graph") ||
      streq(arg, "-fq-compactify-graph=never") ||
      streq(arg, "-fq-compactify-graph=no"))
  {
    shift(argc, argv);
    compactify_graph = CG_NEVER;
    return;
  }
  if (streq(arg, "-fq-compactify-graph=always"))
  {
    shift(argc, argv);
    compactify_graph = CG_ALWAYS;
    return;
  }

  HANDLE_FLAG(module_access, "-fq-", "module-access");
  HANDLE_FLAG(module_write, "-fq-", "module-write");
  HANDLE_FLAG(module_trust, "-fq-", "module-trust");
  HANDLE_FLAG(module_print_class2mod, "-fq-", "module-print-class2mod");
}

void QualCmd::dump() {
  OinkCmd::dump();
  printf("q-config '%s'\n", config);
  printf("q-hotspots: %d\n", hotspots);
  printf("fq-print-quals-graph: %s\n", boolToStr(print_quals_graph));
  printf("fq-strict-const: %s\n", boolToStr(strict_const));
  printf("fq-casts-preserve: %s\n", boolToStr(casts_preserve));
  printf("fq-use-const-subtyping: %s\n", boolToStr(use_const_subtyping));
  printf("fq-ugly: %s\n", boolToStr(ugly));
  printf("fq-poly: %s\n", boolToStr(poly));

  // qual arguments
  printf("q-catch-qual:\n");
  FOREACH_ASTLIST(char, catch_qual, iter) {
    printf("\t%s\n", iter.data());
  }
  printf("q-exclude-global:\n");
  FOREACH_ASTLIST(char, exclude_global, iter) {
    printf("\t%s\n", iter.data());
  }
  printf("q-exclude-cast:\n");
  FOREACH_ASTLIST(char, exclude_cast, iter) {
    printf("\t%s\n", iter.data());
  }
  printf("q-func-param-ret-qual:\n");
  FOREACH_ASTLIST(char, func_param_ret_qual, iter) {
    printf("\t%s\n", iter.data());
  }

  printf("reg-stack-regfunc: %s\n",
         (reg_stack_regfunc ? reg_stack_regfunc : "null"));
  printf("reg-stack-un-regfunc: %s\n",
         (reg_stack_un_regfunc ? reg_stack_un_regfunc : "null"));
  printf("reg-stack-array-regfunc: %s\n",
         (reg_stack_array_regfunc ? reg_stack_array_regfunc : "null"));
  printf("reg-stack-array-un-regfunc: %s\n",
         (reg_stack_array_un_regfunc ? reg_stack_array_un_regfunc : "null"));

  printf("q-const-when-param:\n");
  FOREACH_ASTLIST(QSpec, const_when_param, iter) {
    printf("\t%s\n", iter.data()->toString());
  }
  printf("q-reg-stack:\n");
  FOREACH_ASTLIST(QSpec, reg_stack, iter) {
    printf("\t%s\n", iter.data()->toString());
  }
  printf("q-exl-perm-alias:\n");
  FOREACH_ASTLIST(QSpec, exl_perm_alias, iter) {
    printf("\t%s\n", iter.data()->toString());
  }
  printf("q-exl-value-ref-call:\n");
  FOREACH_ASTLIST(QSpec, exl_value_ref_call, iter) {
    printf("\t%s\n", iter.data()->toString());
  }
  printf("q-arg-local-var:\n");
  FOREACH_ASTLIST(QSpec, arg_local_var, iter) {
    printf("\t%s\n", iter.data()->toString());
  }
  printf("q-temp-immediately-used:\n");
  FOREACH_ASTLIST(QSpec, temp_immediately_used, iter) {
    printf("\t%s\n", iter.data()->toString());
  }

  printf("fq-inference: %s\n", boolToStr(inference));
  printf("fq-print-trans-qual: %s\n", boolToStr(print_trans_qual));
  printf("fq-name-expressions: %s\n", boolToStr(name_expressions));
  printf("fq-name-vars: %s\n", boolToStr(name_vars));
  printf("fq-name-if_missing: %s\n", boolToStr(name_if_missing));
  printf("fq-casts-preserve-below-functions: %s\n",
         boolToStr(casts_preserve_below_functions));

  printf("fq-flow-compoundUp: %s\n", boolToStr(flow_compoundUp));
  printf("fq-flow-compoundDown: %s\n", boolToStr(flow_compoundDown));
  printf("fq-flow-pointUp: %s\n", boolToStr(flow_pointUp));
  printf("fq-flow-pointDown: %s\n", boolToStr(flow_pointDown));
  printf("fq-flow-refUp: %s\n", boolToStr(flow_refUp));
  printf("fq-flow-refDown: %s\n", boolToStr(flow_refDown));

  printf("fq-stackness: %s\n", boolToStr(stackness));

  printf("fq-explain-errors: %s\n", boolToStr(explain_errors));
  printf("q-max-errors: %d\n", max_errors);
  printf("fq-ret-inf: %s\n", boolToStr(ret_inf));
  printf("fq-name-with-loc: %s\n", boolToStr(name_with_loc));
  printf("fq-name-with-serialno: %s\n", boolToStr(name_with_serialno));

  printf("fq-merge-ref-ptr-qvars: %s\n", boolToStr(merge_ref_ptr_qvars));

  printf("fq-compactify-graph: %s\n",
         compactify_graph == CG_ALWAYS ? "always" :
         compactify_graph == CG_IFSRZ ? "ifsrz" :
         compactify_graph == CG_NEVER ? "never" :
         "?!");

  printf("fq-module-access: %s\n", boolToStr(module_access));
  printf("fq-module-write: %s\n", boolToStr(module_write));
  printf("fq-module-trust: %s\n", boolToStr(module_trust));
  printf("fq-module-print-class2mod: %s\n", boolToStr(module_print_class2mod));
}

void QualCmd::printHelp() {
  OinkCmd::printHelp();
  printf
    ("%s",
     "\n\nqual/legacy-cqual flags that take an argument:\n"
     "  -q-config FILE           : set the configuration file to FILE\n"
     "                                 (can also set $QUALCC_CONFIG)\n"
     "  -q-hotspots NUM          : set the number of hotspots to NUM\n"
     "\n"
     "qual/legacy-cqual boolean flags; precede by '-fq-no-' for the negative sense.\n"
     "  -fq-print-quals-graph    : print out quals.dot of inference graph\n"          //
     "  -fq-strict-const         : enforce const, rather than allow const inference\n"
     "  -fq-casts-preserve       : don't notify the libqual backend about flow through a cast\n"
     "  -fq-use-const-subtyping  : no back edge below pointers if pointing to const\n"
     "  -fq-ugly                 : put address of qvar into quals.dot node names\n"
     "  -fq-poly                 : do polymorphic analysis\n"
     "\n"
     "qual flags that take an argument:\n"
     "A QLIT is a qualifier literal: $tainted\n"
     "  -q-catch-qual QLIT       : attach qualifier literal QLIT to the global exception var\n"

     // NOTE: This is currently a one-off for Mozilla; I reserve the
     // right to change the semantics of this function.  Document this
     // on the qual arguments documentation page.
     "  -q-exclude-global QLIT   : exclude global declarators containing qualifier QLIT\n"
     "  -q-exclude-cast QLIT     : exclude cast declarators containing qualifier QLIT\n"
     // FIX: get rid of this
//      "  -q-func-param-ret-qual QLIT : attach qualifier literal QLIT to the abstr value tree\n"
//      "                             of the parameters and return value of all functions\n"
     "  -q-reg-stack-regfunc FUNC : name of registration function\n"
     "  -q-reg-stack-un-regfunc FUNC : name of un-registration function\n"
     "  -q-reg-stack-array-regfunc FUNC : name of array registration function\n"
     "  -q-reg-stack-array-un-regfunc FUNC : name of array un-registration function\n"
     "\n"
     "qual flags that take a Qualifier Specification (QSPEC) argument.\n"
     "A QSPEC is a QLIT and a suffix of some stars indicating ptr/array levels: $tainted*\n"
     "  -q-const-when-param QSPEC : mark Values as const when they match the qspec and \n"
     "                             occur in a parameter variable\n"
     "  -q-reg-stack QSPEC       : check stack declarations of variables with qualifier QSPEC\n"
     "                             follow the register-use-unregister discipline;\n"
     "                             see -q-reg-* flags to set names of the registration funcs\n"
     "  -q-exl-perm-alias QSPEC  : check expressions with qualifier QSPEC must occur in the\n"
     "                             context of a function argument or pointer dereference\n"
     "  -q-exl-value-ref-call QSPEC : check variables of type QSPEC may not occur in a\n"
     "                             FullExpression both:\n"
     "                             1) as an r-value function argument and\n"
     "                             2) in a reference context or where its address is taken\n"
     "  -q-arg-local-var QSPEC   : check that function arguments of type QSPEC\n"
     "                             are stack or parameter allocated variables\n"
     "  -q-temp-immediately-used QSPEC : check that an expression of type QSPEC\n"
     "                             is a stack- or param-alloc var, or are in the\n"
     "                             context of 1) a dereference or 2) an initializer\n"

     // FIX: "MAX" should be added to the qual arguments documentation page
     // FIX: doesn't seem to be printed in the verbose section
     "  -q-max-errors MAX        : max errors to print; 0 for unlimited (default 80)\n"

     "\n"
     "qual boolean flags; preceded by '-fq-no-' for the negative sense.\n"
     "  -fq-inference            : do inference\n"
     "  -fq-print-trans-qual     : in pretty printing, annotate inferred qualifiers\n"
     "  -fq-name-expressions     : name expressions\n"
     "  -fq-name-vars            : name variables\n"
     "  -fq-name-if-missing      : name otherwise unnamed objects\n"
     "  -fq-names                : control all other -fq-name flags\n"
     "  -fq-casts-preserve-below-functions : -fcasts-preserve works below functions\n"
     "  -fq-flow-compoundUp      : insert var to container edges\n"
     "  -fq-flow-compoundDown    : insert container to var edges\n"
     "  -fq-flow-pointUp         : insert value to pointer/array edges\n"
     "  -fq-flow-pointDown       : insert pointer/array to value edges\n"
     "  -fq-flow-refUp           : insert value to ref edges\n"
     "  -fq-flow-refDown         : insert ref to value edges\n"

     "  -fq-stackness            : prohibit global/heap pointers to stack\n"

     "  -fq-explain-errors       : print bad dataflow path when one is found\n"
     "  -fq-ret-inf              : return the inference result in the exit code\n"
     "  -fq-name-with-loc        : put location info on qualifier variable names\n"
     "  -fq-merge-ref-ptr-qvars  : merge qvars of refs and ptrs (vs unify)\n"
     "  -fq-compactify-graph     : compactify graph before serialization\n"
     "  -fq-compactify-graph=always : compactify graph even if not serializing\n"
     "\n"
     "module analysis: look for violations of the module boundaries.\n"
     "  -fq-module-access        : other module accesses a module's memory\n"
     "  -fq-module-write         : other module writes a module's memory\n"
     "  -fq-module-trust         : access through a pointer in another's control"
     "  -fq-module-print-class2mod : print map from classes to modules"
     "");
}

// push the state out to other places where it is needed; return value
// is true unless there is an error
void QualCmd::initializeFromFlags() {
  OinkCmd::initializeFromFlags();

  // hotspots
  if (hotspots < 0) {
    throw UserError(USER_ERROR_ExitCode,
                    stringc << "hotspots must be non-negative: " << hotspots);
  }
  LibQual::num_hotspots = hotspots;

  // print_quals_graph
  LibQual::flag_print_quals_graph = print_quals_graph ?1:0;

  // ugly
  LibQual::flag_ugly = ugly ?1:0;

  // poly
  LibQual::flag_poly = poly ?1:0;

  // explain errors
  LibQual::flag_explain_errors = explain_errors ?1:0;

  // max errors
  LibQual::flag_max_errors = max_errors;

  // check for inconsistent command line combinations
  if (exit_after_parse && print_quals_graph) {
    throw UserError(USER_ERROR_ExitCode,
                    "You may not use -fq-exit-after-parse and -fprint-quals-graph together.");
  }

  if (exit_after_typecheck && print_quals_graph) {
    throw UserError(USER_ERROR_ExitCode,
                    "You may not use -fq-exit-after-typecheck and -fprint-quals-graph together.");
  }

  if (exit_after_elaborate && print_quals_graph) {
    throw UserError(USER_ERROR_ExitCode,
                    "You may not use -fq-exit-after-elaborate and -fprint-quals-graph together.");
  }

  if (instance_sensitive && print_trans_qual) {
    throw UserError(USER_ERROR_ExitCode,
                    "You may not use -fo-instance-sensitive and -fq-print-trans-qual together.");
  }

  if (instance_sensitive && globalLang.isCplusplus) {
    throw UserError(USER_ERROR_ExitCode,
                    "You may not use -fo-instance-sensitive in C++ mode.");
  }

  if (!pretty_print && print_trans_qual) {
    throw UserError(USER_ERROR_ExitCode,
                    "You must use -fo-pretty-print with -fq-print-trans-qual.");
  }

  if (reg_stack.count() > 0) {
    if (!reg_stack_regfunc) {
      throw UserError
        (USER_ERROR_ExitCode, "if -q-reg-stack is used, -q-reg-stack-regfunc must be set");
    }
    if (!reg_stack_un_regfunc) {
      throw UserError
        (USER_ERROR_ExitCode, "if -q-reg-stack is used, -q-reg-stack-un-regfunc must be set");
    }
    if (!reg_stack_array_regfunc) {
      throw UserError
        (USER_ERROR_ExitCode, "if -q-reg-stack is used, -q-reg-stack-array-regfunc must be set");
    }
    if (!reg_stack_array_un_regfunc) {
      throw UserError
        (USER_ERROR_ExitCode, "if -q-reg-stack is used, -q-reg-stack-array-un-regfunc must be set");
    }
  }

  // NOTE: flattenInputFiles() has already been called
  // this is off because I am now using the same input file mechanism for the .qdir files
//    if (srz && inputFiles.count() > 1) {
//      throw UserError(USER_ERROR_ExitCode,
//                      "You may not use -q-srz with more than one input file.");
//    }
}
