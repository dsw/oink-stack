// see License.txt for copyright and terms of use

// Qualifier analysis: This is the flagship Oink tool that hooks Scott
// McPeak's C++ front-end elsa, the oink dataflow functionality, and
// Rob Johnson's libqual backend together into a polymorphic qualifier
// analysis for C and C++.
//
// Specifically, the primary functionality that qual adds to oink is
// here.  This module provides implementation of each stage of
// processing, but leaves it to a main() function to call those
// methods.

#ifndef QUAL_H
#define QUAL_H

#include "oink.h"
#include "oink_global.h"        // globalLang
#include "qual_funky.h"         // DeepLiteralsList
#include "qual_value_print.h"   // QualCTypePrinter
#include "dataflow_visitor.h"   // DataFlowVisitor
#include "qual_libqual_iface.h" // LibQual::
#include "archive_srz.h"

class QualVisitor;
class DataFlowEx;

// An instance of NameASTNodesVisitor is embedded in Qual, so this
// class definition has to come here.

// class NameASTNodesVisitor ****************

// visitor for attaching names to qualifiers of values that annotate
// AST nodes
class NameASTNodesVisitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()

  explicit NameASTNodesVisitor()
    : loweredVisitor(this)
  {}
  virtual bool visitExpression(Expression *);
  // prune the walk at filtered nodes
  virtual bool visitDeclarator(Declarator *obj);
  virtual bool visitFunction(Function *obj);
};

#if DEBUG_INSTANCE_SPECIFIC_VALUES
// class MarkDecltorsInstanceSpecificVisitor ****************

// visitor for marking Declarators instance-specific
class MarkDecltorsInstanceSpecificVisitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()

  explicit MarkDecltorsInstanceSpecificVisitor()
    : loweredVisitor(this)
  {}
  bool visitDeclarator(Declarator *obj);
  // NOTE: do not prune this visitor at Functions and Declarators for
  // filtered Variables
};
#endif

// Attach qualifiers to function parameters and return values.
class FuncParamRetQual_ValueVisitor : public ValueVisitor {
  public:
  DataFlowEx &dfe;              // for adding dataflow edges

  private:
  SObjSet<Value*> visitedValue; // set for idempotent visiting

  public:
  FuncParamRetQual_ValueVisitor(DataFlowEx &dfe0)
    : dfe(dfe0)
  {}
  virtual ~FuncParamRetQual_ValueVisitor() {}

  virtual bool preVisitValue(Value *obj);

  virtual bool preVisitVariable(Variable_O *var) {
    // in particular, we do want to decend into the parameter lists of
    // FunctionValues
    return true;
  }
};


// class Qual ****************

class Qual : public virtual Oink {
  public:
  PtrMap<char const, DeepLiteralsList> globalDeepLiteralsLists;
  PtrMap<char const, void> supportedOperators;
  PtrSet<AtomicType> usedAtomicTypes;
  LibQual::Type_qualifier *thrownQv; // qvar for all layers of all thrown types
  DataFlowEx &dfe;
  long numExternQvars;

  // map fully qualified names of classes to their modules
  StringRefMap<char const> *classFQName2Module;
  // list of class typedef variables
  SObjList<Variable_O> *classVars;

  // tor ****
  public:
  Qual(DataFlowEx &dfe0)
    : usedAtomicTypes()
    , thrownQv(NULL)
    , dfe(dfe0)
    , numExternQvars(-1)
    , classFQName2Module(NULL)
    , classVars(NULL)
  {}

  // methods ****
  void configure();

  virtual void printStats();
  virtual void printSizes();
  virtual const char *getBackendName() const { return "qual"; }
  virtual void srzFormat(ArchiveSrzFormat &srzfmt, bool writing);

  // deserialization
  void deserialize_1archive(ArchiveDeserializer *arc, XmlReaderManager &manager);
  void deserialize_abstrValues(ArchiveDeserializer* arc, XmlReaderManager &manager);
  void deserialize_valueQualMap_stream
  (std::istream& in, XmlReaderManager &manager, LibQual::s18n_context *serializationCtxt);
  void deserialize_valueQualMapAndQualGraph(ArchiveDeserializer* arc, XmlReaderManager &manager);

  // serialization
  void markExternVars();
  void compactifyGraph();

  void serialize_abstrValues(ArchiveSerializer* arc) {xassert(false && "should never be called");}
  int serialize_valuesAndMap(ArchiveSerializer* arc, LibQual::s18n_context *serializationCtxt);
  void serialize_results();

  void visitInSerializationOrder(ValueVisitor &visitor);

  // linking
  virtual bool varUsedInDataflow(Variable_O *var);
  virtual void unifyVars(Variable_O *v1, Variable_O *v2, SourceLoc loc);

  // stages
  void markInstanceSpecificValues_stage();

  void ensure_classFQName2Module();
  void moduleAlloc_stage();
  void moduleOtherControl_stage();
  void moduleOtherWrite_stage();
  void moduleAccess_stage();
  void moduleOtherAccess_stage();
  void moduleAnalysis_stages();

  void exclude_qual_stage();
  void qualCompile_stage();

  void qualCompile_setGlobalness();
  void qualCompile_nameExpressions();
  void qualCompile_nameVars();
  void doNameVars();
  void doNameExpressions();
  void doNameUnnamed();
  void doAssignNames();
  void qualCompile_qualVisitation();

  bool finishQuals_stage();
  void markExtern_stage();
  void compactify_stage();

  void updateEcrAll();
  void removeDeadQvars();
};

#endif // QUAL_H
