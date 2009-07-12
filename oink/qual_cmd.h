// see License.txt for copyright and terms of use

// Process command-line arguments.

#ifndef QUAL_CMD_H
#define QUAL_CMD_H

#include "oink_cmd.h"
#include "oink_file.h"          // File

// a qualifier specifier, see QSPEC in the user documentation below
class QSpec {
  public:
  char const * const original;  // the original string
  char const * const qual;      // the raw qualifier
  int const depth; // the depth of pointed-ness == number of stars in spec string

  public:
  QSpec(char const *original0, char *qual0, int depth0);

  public:
  char const *toString() const { return original; }
  void dump();                  // debugging
};

class QualCmd : public virtual OinkCmd {
  public:
  // cqual arguments
  char *config;                 // lattice configuration file

  // FIX: test this; it should produce some output under some
  // circumstances.  With print quals graph?
  int hotspots;

  bool print_quals_graph;       // works
  bool strict_const;            // works
  bool casts_preserve;          // works
  bool use_const_subtyping;     // works
  bool flow_sensitive;          // waiting for cqual people to get it done
  bool ugly;                    // works
  bool poly;                    // works; passed to polymorphic backend

  // qual arguments

  // Literals to attach to the generic thrown qvar.  These are
  // qualifiers for a "generic" or "universal" catch clause.
  ASTList<char> catch_qual;
  // Exclude global declarators containing the given qualifier.
  ASTList<char> exclude_global;
  // Exclude cast declarators containing the given qualifier.
  ASTList<char> exclude_cast;
  // Literals to attach qualifier to the abstr value tree of the
  // parameters and return value of all functions.
  ASTList<char> func_param_ret_qual;

  // **** Mozilla analyses

  // names of registration functions for the implementation of
  // -q-reg-stack
  char * reg_stack_regfunc;
  char * reg_stack_un_regfunc;
  char * reg_stack_array_regfunc;
  char * reg_stack_array_un_regfunc;

  // mark Values as const when they match the qspec and occur in a
  // parameter variable
  ASTList<QSpec> const_when_param;

  // check stack declarations of pointers to variables with this
  // qualifier must be registered with a call to register(&v)
  // immediately after the declaration
  ASTList<QSpec> reg_stack;

  // check expressions with qualifier QLIT ** must occur in the
  // context of a function argument or a pointer dereference
  ASTList<QSpec> exl_perm_alias;

  // check a variable with type QLIT * may not occur in a
  // FullExpression both: 1) as an r-value function argument and 2) in
  // a reference context or where its address is taken
  ASTList<QSpec> exl_value_ref_call;

  // check that function arguments of type QLIT* are stack or
  // parameter allocated variables
  ASTList<QSpec> arg_local_var;

  // check that an expression of type 'qual*' is a stack- or
  // parameter-allocated variable, or in the context of a dereference,
  // or in the context of an initializer
  ASTList<QSpec> temp_immediately_used;

  // ****

  bool inference;               // do inference
  bool print_trans_qual;        // print with transitive qualifier deductions
  bool name_expressions;        // whether to name expressions
  bool name_vars;               // whether to name variables
  bool name_if_missing;         // whether to name unnamed items

  // when casting between function types, keep going down into their
  // return and parameter types
  bool casts_preserve_below_functions;

  // do we insert structural edges?
  bool flow_compoundUp;         // edge up from var to compound
  bool flow_compoundDown;       // compound edge down
  bool flow_pointUp;            // edge up from pointed-to to pointer
  bool flow_pointDown;          // pointer edge down
  bool flow_refUp;              // edge up from refered-to to ref
  bool flow_refDown;            // ref edge down

  // ad-hoc analyses
  bool stackness;               // do stackness analysis

  bool explain_errors;          // print bad dataflow path when one is found
  int max_errors;               // max errors
  bool ret_inf;                 // exit with error code when a qualifier inconsistency is detected
  bool name_with_loc;           // type names should contain a location string also
  bool name_with_serialno;      // type names should contain a serial number also

  bool merge_ref_ptr_qvars;     // whether to merge ref and ptr qvars instead of unifying

  // module analysis flags
  bool module_access;           // do the module access analysis
  bool module_write;            // do the module write analysis
  bool module_trust;            // do the module trust analysis

  // compactify graph before serialization
  enum { CG_NEVER, CG_ALWAYS, CG_IFSRZ } compactify_graph;

  QualCmd();

  virtual void readEnvironment();

  void readOneArg(int &argc, char **&argv);
  void dump();
  void printHelp();
  void initializeFromFlags();

  // convenience methods
  bool doCompactifyGraph() {
    return
      compactify_graph == QualCmd::CG_ALWAYS ||
      (compactify_graph == QualCmd::CG_IFSRZ && !srz.empty());
  }
};

#endif // QUAL_CMD_H
