// see License.txt for copyright and terms of use

// The primary functionality that oink proper (not including dataflow
// or qual) adds to elsa.  Provides implementation of each stage of
// processing, but leaves it to a main() function to call those
// methods.  Much here can be re-used by other analyses by
// inheritance.

#ifndef OINK_H
#define OINK_H

#include "cc_type.h"
#include "cc_ast_aux.h"         // LoweredASTVisitor
#include "oink_var.h"           // Variable_O
#include "oink_file.h"          // File
#include "oink_cmd.h"           // InputLang
#include "value_print.h"        // ValueTypePrinter
#include "xml_value_writer.h"   // VarPredicate
#include "cc_lang.h"            // CCLang
#include "useract.h"            // UserActions
#include "ccparse.h"            // ParseEnv
#include "ptrmap.h"             // PtrMap
#include "taillist.h"           // TailList

class XmlReaderManager;         // elsa/xml_reader.h

class ArchiveDeserializer;      // archive_srz.h
class ArchiveSerializer;

class ArchiveSrzFormat;         // archive_srz_format.h

// forwards in this file
class Oink;
class FuncGranGraph;

class Linker {
public:
  // global map from the fully qualified extern names to the extern
  // variables
  typedef TailList<Variable_O> VarList;
  StringRefMap<VarList> extern_name2vars;
  StringRefMap<VarList> tolink_name2vars;

  // The linker-visible variables that during linking were marked as the
  // first one in their equivalence class that was used in the dataflow.
  // Equivalent to mapcar (value.list.first()) tolink_name2vars.
  PtrSet<Variable_O> linkerVisVarsToBeSrz;

  // Whether linking has happened; tolink_name2vars and linkerVisVarsToBeSrz
  // are valid in this case.
  bool haveLinked;

  Linker() : haveLinked(false) {}
  ~Linker();

  // add a varible to the Linker
  void add(Variable_O *var);

  // Returns the variable to link, or NULL if none defined.
  Variable_O *getFunctionForLinking(const char *name);

  // return a list of variables to serialize.
  void getOrderedExternVars(VarPredicate *varPred, TailList<Variable_O> &externVars);

  // // get the funcDefn if any for this variable across translation
  // // units
  // Function *wholeProgram_funcDefn(Variable_O *var);
};

// I think perhaps this is just an abstract control flow graph
class FuncGranGraph {
  public:
  // directed graph from Variables on Functions (AST function
  // definitions) to Variables in E_variable expressions
  PtrMap<Variable, SObjSet<Variable*> > flowsTo;
  Variable *root;               // root of the graph for the global "super-main"

  explicit FuncGranGraph(Variable *root0) : root(root0) {
    xassert(root);
  };

  Variable *getRoot() { return root; }
  SObjSet<Variable*> *getRootToSet() { return flowsTo.get(root); }
  void addEdge(Variable *from, Variable *to);
};

class FuncGranASTVisitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()

  FuncGranGraph &funcGranGraph;

  protected:
  // The stack of nested functions
  SObjStack<Function> functionStack;

  // ctor ****
  public:
  explicit FuncGranASTVisitor(FuncGranGraph &funcGranGraph0)
    : loweredVisitor(this)
    , funcGranGraph(funcGranGraph0)
  {}
  virtual ~FuncGranASTVisitor() {}

  // utility methods
  protected:
  void dependOnCurrentFunc(Variable *to);
  void dependOnSuperMain(Variable *to);
  void visitArgumentList(FakeList<ArgExpression> *args);

  // manually called visit methods
  protected:
  virtual bool visitFunction_once_initial(Function *);
  virtual bool visitFunction_once_decl(Function *) {return true;}
  virtual bool visitFunction_once_body(Function *) {return true;}

  // methods called by traverse()
  public:
  bool visitExpression(Expression *obj);
  bool visitInitializer(Initializer *obj);
  bool visitMemberInit(MemberInit *obj);
  bool visitFunction(Function *obj);
};

// visit real vars and honor the filter
class VisitRealVars_filter : public VisitRealVars {
  // tor
  public:
  explicit VisitRealVars_filter(visitVarFunc_t *visitVarFunc0)
    : VisitRealVars(visitVarFunc0)
  {}

  // methods
  virtual bool shouldVisitVariable(Variable *var) {
    return !asVariable_O(var)->filteredOut();
  }
};

void visitVarsMarkedRealF_filtered(ArrayStack<Variable*> &builtinVars, VisitRealVars &visitReal);
void visitRealVarsF_filtered(TranslationUnit *tunit, VisitRealVars &visitReal);

// skip function bodies and Declarators of filtered Functions
class RealVarAndTypeASTVisitor_filtered : public RealVarAndTypeASTVisitor {
  public:
  explicit RealVarAndTypeASTVisitor_filtered(VariableVisitor *variableVisitor0 = NULL)
    : RealVarAndTypeASTVisitor(variableVisitor0)
  {}
  explicit RealVarAndTypeASTVisitor_filtered(TypeVisitor *typeVisitor0)
    : RealVarAndTypeASTVisitor(typeVisitor0)
  {}
  explicit RealVarAndTypeASTVisitor_filtered
    (VariableVisitor *variableVisitor0, TypeVisitor *typeVisitor0)
    : RealVarAndTypeASTVisitor(variableVisitor0, typeVisitor0)
  {}

  public:
  virtual bool visitDeclarator(Declarator *obj);
  virtual bool visitFunction(Function *obj);
};

class Oink {
  public:
  UserActions *parseUserActions;
  ParseEnv *parseEnv;           // same obj as 'userActions' due to Scott's bizarre parsing setup
  // ParseTables *parseTables;
  ValueTypePrinter *typePrinter;
  PtrMap<File, TranslationUnit> file2unit; // map files to translation units
  ArrayStack<Variable*> madeUpVariables; // variables made-up in the course of typechecking
  ArrayStack<Variable*> builtinVars; // builtin variables
  Variable *funcGranRoot;       // root of the function-granularity CFG
  FuncGranGraph funcGranGraph; // function-granularity control flow graph
  SourceLocManager mgr;

  class ArchiveSerializationManager *archiveSrzManager;

  // tor ****
  Oink();
  virtual ~Oink();

  // methods ****

  void printBuckets();
  void printStage(const char *name);
  void printStage0(const char *name);
  virtual const char *getBackendName() const { return "oink"; }
  virtual void printStats();
  virtual void printSizes();
  virtual void printProcStats();

  bool isSource(File const *file);
  virtual bool isSerialized(File const *file);
  void maybeSetInputLangFromSuffix(File *file);
  void loadControlFile();
  void checkInputFilesRecognized();

  // stages
  void parseOneFile(File *file, ParseTables *parseTables);
  void typecheckOneFile(File *file);
  void astPrintOneFile(File *file);

  void init_stage(int argc, char **argv);
  void parse_stage();           // parse
  void typecheck_stage();       // typecheck
  void elaborate_stage();       // elaborate
  void markRealVars_stage();    // mark real (non-template) vars as such
  void value_stage();           // annotate with values
  void registerVars_stage();    // register variables with the linker
  void integrity_stage();       // check various invariants
  void exclude_syntax_stage();  // exclude syntactic combinations
  void simpleDataflow_stage();  // compute simplified dataflow for use in linking
  void checkForUsedVars_stage(); // warn if a var was used that a control says to warn about
  void deserialize_stage();     // get linker-visible variables and annotations back from file
  void link_stage();            // link
  void prettyPrint_stage();     // pretty print

  // function granularity CFG
  void compute_funcGran();      // compute function granularity CFG
  void printVariableName_funcGran(ostream &out, Variable *var);
  void printVariableAndDep_funcGran(ostream &out, Variable *from, SObjSet<Variable*> *toSet);
  void printVariableAndDep_DOT_funcGran(ostream &out, Variable *from, SObjSet<Variable*> *toSet);
  void output_funcGran(ostream &out, bool dot); // output function granularity CFG
  void print_funcGran();        // print function granularity CFG

  // Variable filtering
  void loadFuncFilterNames();   // load func_filter into funcFilterNames
  void markFuncFilterVars(void (*marker)(Variable*)); // mark Variables that have been filtered
  // NOTE: even if you are not filtering Variables out, you have to
  // run this stage anyway to make all vars as setFilteredKeep(true)
  // as the default is false
  void filter_stage();
  void pruneFilteredBuiltins();

  // linking
  void linkVariables(Linker::VarList &vars);
  virtual bool varUsedInDataflow(Variable_O *var);
  virtual void unifyVars(Variable_O *v1, Variable_O *v2, SourceLoc loc) {}

  private:
  int linkGetVariablesToLink(Linker::VarList &vars,
                             Linker::VarList &varsToLink);
  void linkCheckErrors(int linkErrors);

  // serialization/deserialization
  public:
  void initArchiveSrzManager(const char *ext_dir, const char *ext_zip);
  virtual void srzFormat(ArchiveSrzFormat &srzfmt, bool writing);

  void serialize_results();
  void serialize_formatVersion(ArchiveSerializer * arc);
  void serialize_files(ArchiveSerializer * arc);
  void serialize_files_stream(ostream &out);
  void serialize_abstrValues(ArchiveSerializer* arc);
  void serialize_abstrValues_stream(XmlValueWriter &valueWriter, VarPredicate *varPred);

  void* expectOneXmlTag(XmlReaderManager &manager, int expectKind);
  virtual void deserialize_1archive(ArchiveDeserializer *arc, XmlReaderManager &manager);
  void deserialize_formatVersion(ArchiveDeserializer * arc);
  void deserialize_files(ArchiveDeserializer *arc, XmlReaderManager &manager);
  void deserialize_files_stream
    (XmlReaderManager &manager, istream &in, const char* fname, const char *archiveName);
  void deserialize_abstrValues(ArchiveDeserializer *arc, XmlReaderManager &manager);
  void deserialize_abstrValues_toLinker(ArchiveDeserializer *arc, XmlReaderManager &manager);
  SObjList<Variable_O> *deserialize_abstrValues_stream
    (XmlReaderManager &manager, istream& in, const char* fname);
};

void printStart(char const *name);
void printStop();

#define foreachFile \
  for(ASTListIterNC<File> files(oinkCmd->inputFilesFlat); !files.isDone(); files.adv())

#define foreachSourceFile foreachFile if(isSource(files.data()))
#define foreachSerializedFile foreachFile if(isSerialized(files.data()))

#endif // OINK_H
