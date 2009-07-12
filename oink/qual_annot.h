// see License.txt for copyright and terms of use

// Annotations onto the *Value_Q types that hold the libqual backend
// annotations.  The idea is to keep the libqual backend
// implementation of the oink/qual_* modules somewhat separated from
// oink/qual_* itself.

#ifndef QUAL_ANNOT_H
#define QUAL_ANNOT_H

#include "qual_funky.h"
#include "qual_libqual_iface.h" // LibQual::
#include "oink_var.h"
#include "value.h"
#include "packedword.h"

// forwards in this file
class CVAtomicValue_Q;
class PointerValue_Q;
class ReferenceValue_Q;
class FunctionValue_Q;
class ArrayValue_Q;
class PointerToMemberValue_Q;
class ValueFactory_Q;

#define FOREACH_CLASS_QualAnnot(D)                                \
  D(CVAtomicValue_QualAnnot)                                      \
  D(PointerValue_QualAnnot)                                       \
  D(ReferenceValue_QualAnnot)                                     \
  D(FunctionValue_QualAnnot)                                      \
  D(ArrayValue_QualAnnot)                                         \
  D(PointerToMemberValue_QualAnnot)

class DataFlowEx;

enum Globalness {
  UNDEF_Globalness = 0,         // must be 0, otherwise initialize in QualAnnot
  GLOBAL_Globalness,
  LOCAL_Globalness,
};
string globalnessToString(Globalness globalness0);

class QualAnnot_ExternVisit : public ValueVisitor {
  bool externness;
  int counter;

public:
  QualAnnot_ExternVisit(bool externness0) : externness(externness0), counter(0) {}

  virtual bool preVisitValue(Value *value);
  virtual bool preVisitVariable(Variable_O *var);

  int getCount() const { return counter; }
};

// is this qualifier a special qualifier?
bool isSpecialQualifier(QLiterals *ql0);

// annotations on a value for qualifier inference purposes
class QualAnnot {
  public:

  // NOTE: vname and globalness are properties stored on the qvar qv0; we keep
  // them here so that we don't need to make a qv0 to put them onto when we
  // get them as that is wasteful; that is, we decouple getting these values
  // with pushing them out to the qvar.  Note that these properties can in
  // fact be out of sync with the properties on qv0, such as after
  // de-serialization; such a situation is not an error and the properties on
  // the qv0 qvar always take priority.

#if !DELAY_SET_NAMES
  const char *vname;
#endif

private:
  // quarl 2006-06-23
  //    To save space, we pack the following objects into a single word:
  //       Globalness globalness ( 2 bits)
  //       bool edgesAdded       ( 1 bit)
  //       Funky funk0           (29 bits)

  // FIX: get rid of edgesAdded when everything is staged; it is just to
  // prevent an infinite loop for now

  PackedWord PW_globalness_edgesAdded_funk;

  PACKEDWORD_DEF_GS(PW_globalness_edgesAdded_funk, Funk0, int, 0, 29)
  PACKEDWORD_DEF_GS(PW_globalness_edgesAdded_funk, Globalness, Globalness, 29, 31)
  PACKEDWORD_DEF_GS(PW_globalness_edgesAdded_funk, EdgesAdded, bool, 31, 32)

  // for grep:
  //   getGlobalness, setGlobalness, getEdgesAdded, setEdgesAdded,
  //   getFunk0, setFunk0

  private:
  // cqual qualifier variable; pointer into the cqual world
  LibQual::Type_qualifier *qv0;

  public:
  friend class Qual;
  friend class QualSerialization_ValueVisitor;

  QualAnnot();
  virtual ~QualAnnot(){}

  LibQual::Type_qualifier *qv();
  bool hasQv();

  // Update to point to ecr qvar.  This is necessary before calling
  // eliminate_quals_links().
  void ecrUpdate();

  // Remove qvar if it's dead.
  void removeQvarIfDead();

  // you shouldn't use this unless you are doing deserialization
  void setQv_deserialize(LibQual::Type_qualifier *qv1);
  bool qvarIsExtern();

  virtual Value *v0() = 0;
  QLiterals *getQl() { return v0()->t()->ql; }

  Funky getFunk();
private:
  Funky attachFunky(SourceLoc loc);
public:
  void attachLiteralsTo(QualAnnot *target);
  void attachLiterals();
  void attachOneLiteral(SourceLoc loc, LibQual::Type_qualifier *qconst);
  LibQual::Type_qualifier *getQvarOfInterest(LibQual::Type_qualifier *qconst);
  void attachOneLiteralIfInLattice(SourceLoc loc, char const *qconst_name);
  LibQual::Type_qualifier *getQVar();
  string inferredQualifiersToString();
  void setGlobal(Globalness globalness);
protected:
  void setName(char const *name0);
private:
  void setName1(char const *name0);
  void setNameMissing1();
public:
  void maybeSetNameMissingIfMissing();

protected:
#if !DELAY_SET_NAMES
  bool haveQaName() const { return vname != NULL; }
#endif

  public:
  virtual void nameTree(char const * name) = 0;
#if !DELAY_SET_NAMES
  char const *getName() const;
#endif
  virtual void setGlobalTree(Globalness globalness) = 0;

  // void setExternTree_common(QualAnnot_ExternVisit &exVis);
  // virtual void setExternTree(QualAnnot_ExternVisit &exVis) = 0;
  virtual void printDeepQualifiers() = 0;

  static char const *unknownName;
};

#define VALUE_V_DECL()                                            \
  MyValue *v();                                                   \
  virtual Value *v0();

class CVAtomicValue_QualAnnot : public QualAnnot {
public:
  typedef CVAtomicValue_QualAnnot MyQualAnnot;
  typedef CVAtomicValue_Q MyValue;
  typedef Struct<MyValue, MyQualAnnot> MyStruct;

  VALUE_V_DECL();
  void nameTree(char const * name);
  void setGlobalTree(Globalness globalness);
  // void setExternTree(QualAnnot_ExternVisit &exVis);
  void printDeepQualifiers();
};

class PointerValue_QualAnnot : public QualAnnot {
public:
  typedef PointerValue_QualAnnot MyQualAnnot;
  typedef PointerValue_Q MyValue;
  typedef Struct<MyValue, MyQualAnnot> MyStruct;

  VALUE_V_DECL();
  void nameTree(char const * name);
  void setGlobalTree(Globalness globalness);
  // void setExternTree(QualAnnot_ExternVisit &exVis);
  void printDeepQualifiers();
};

class ReferenceValue_QualAnnot : public QualAnnot {
public:
  typedef ReferenceValue_QualAnnot MyQualAnnot;
  typedef ReferenceValue_Q MyValue;
  typedef Struct<MyValue, MyQualAnnot> MyStruct;

  VALUE_V_DECL();
  void nameTree(char const * name);
  void setGlobalTree(Globalness globalness);
  // void setExternTree(QualAnnot_ExternVisit &exVis);
  void printDeepQualifiers();
};

class FunctionValue_QualAnnot : public QualAnnot {
public:
  typedef FunctionValue_QualAnnot MyQualAnnot;
  typedef FunctionValue_Q MyValue;
  typedef Struct<MyValue, MyQualAnnot> MyStruct;

  VALUE_V_DECL();

  AdjList *funkyAdjList;        // the adjacency list for our funky set
  bool funky_applied;           // has been applied to our subtree

  FunctionValue_QualAnnot()
    : funkyAdjList(NULL)
    , funky_applied(false)
  {}

  // corresponds to FunctionType::getThisCV; isMember() must be true
  QLiterals const *getThisQLiterals();
  void ensureFunkyInternalEdges(SourceLoc loc, DataFlowEx &dfe);
  void ensureFunkyInternalEllipsisEdges
    (SourceLoc loc, Value *ellipsis_param_value, DataFlowEx &dfe);

  void nameTree(char const * name);
  void setGlobalTree(Globalness globalness);
  // void setExternTree(QualAnnot_ExternVisit &exVis);
  void printDeepQualifiers();
  // FIX: the implementation of this no longer makese sense; if you
  // need this again, then you should traverse the function looking
  // for funky qualifiers.
//    bool hasNontrivialFunkyAdjList() {return funkyAdjList!=NULL;}

  AdjList *getFunkyAdjList();
};

class ArrayValue_QualAnnot : public QualAnnot {
public:
  typedef ArrayValue_QualAnnot MyQualAnnot;
  typedef ArrayValue_Q MyValue;
  typedef Struct<MyValue, MyQualAnnot> MyStruct;

  VALUE_V_DECL();
  void nameTree(char const * name);
  void setGlobalTree(Globalness globalness);
  // void setExternTree(QualAnnot_ExternVisit &exVis);
  void printDeepQualifiers();
};

class PointerToMemberValue_QualAnnot : public QualAnnot {
public:
  typedef PointerToMemberValue_QualAnnot MyQualAnnot;
  typedef PointerToMemberValue_Q MyValue;
  typedef Struct<MyValue, MyQualAnnot> MyStruct;

  VALUE_V_DECL();
  void nameTree(char const * name);
  void setGlobalTree(Globalness globalness);
  // void setExternTree(QualAnnot_ExternVisit &exVis);
  void printDeepQualifiers();
};

#undef CHECKED_DOWNCAST_DECL

LibQual::Type_qualifier *findQualAndCheck(char const *name);
void attachOneLiteralToQvar(SourceLoc locOfAttach,
                            LibQual::Type_qualifier *qconst,
                            LibQual::Type_qualifier *qvarOfInterest);

// debugging: print out the qualifier literals for this value
void dumpQl(Value *t);

// If returns true, we don't need to make an edge to it (when a value
// below a pointer).  NOTE: arrays can't be const; an array of const
// ints is handled by the int case.  FIX: Not sure what to do for a
// function, but asking if the implied-this is const is the wrong
// thing to do I'm sure.
bool isConstForInferencePurposes(Value *t);

bool isVolatileForInferencePurposes(Value *t);

// ****

// Map of qualifier annotations.
// extern PtrMap<Value, QualAnnot> qualAnnotMap;

// void annotateWithQualAnnot(Value *v);

// Get the qualifier annotation for a Value and make sure it is not
// NULL
QualAnnot *qa(Value const *v);

// does this Value have a qual annotation?
// bool hasQualAnnot(Value const *v);

#define DECLARE_QA(VALUENAME)                                     \
  VALUENAME ## _QualAnnot        *qa(VALUENAME const *t);         \
  VALUENAME ## _QualAnnot        *qa(VALUENAME##_Q const *t);

// get the qualifier annotation already downcast
DECLARE_QA(CVAtomicValue)
DECLARE_QA(PointerValue)
DECLARE_QA(ReferenceValue)
DECLARE_QA(FunctionValue)
DECLARE_QA(ArrayValue)
DECLARE_QA(PointerToMemberValue)

#undef DECLARE_QA

void qa_removeDeadQvars();

#endif // QUAL_ANNOT_H
