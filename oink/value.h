// see License.txt for copyright and terms of use

// An Abstract Value class.  It is isomorphic to the Constructed Type
// subset of the Elsa typesystem.  The main difference is that they
// are never re-used: the AST together with the Variables, when
// annotated with Values, still forms a tree.  These are the nodes
// that the dataflow analysis uses when creating its edges.

#ifndef VALUE_H
#define VALUE_H

#include "alloc.h"              // IterableBlockAllocator
#include "oink_var.h"           // Variable_O
#include "cc_type.h"            // *Type
#include "cc_print.h"           // OutStream
#include "strsobjdict.h"        // StringSObjDict

// forwards
class CVAtomicValue;
class PointerValue;
class ReferenceValue;
class FunctionValue;
class ArrayValue;
class PointerToMemberValue;

class FVEllipsisHolder;
class DataFlowTy;

#define FOREACH_CLASS_Value(D)                                    \
  D(CVAtomicValue)                                                \
  D(PointerValue)                                                 \
  D(ReferenceValue)                                               \
  D(FunctionValue)                                                \
  D(ArrayValue)                                                   \
  D(PointerToMemberValue)

// the map from values to types will resolve only when this is on
extern bool value2typeIsOn;


// abstract superclass
class ValuePred {
public:
  virtual bool operator() (Value const *t) = 0;
  virtual ~ValuePred() {}     // gcc sux
};

// when you just want a stateless predicate
typedef bool ValuePredicateFunc(Value const *t);

class StatelessValuePred : public ValuePred {
  ValuePredicateFunc * const f;
public:
  explicit StatelessValuePred(ValuePredicateFunc *f0) : f(f0) {}
  virtual bool operator() (Value const *t);
};


class ValueVisitor {
  public:
  virtual ~ValueVisitor() {}    // silence warning

  virtual bool preVisitValue(Value *obj) { return true; }
  virtual void postVisitValue(Value *obj) {}

  // there is bascially nothing to do so I don't see the point in
  // making these methods abstract virtual
  virtual bool preVisitFVEllipsisHolder(FVEllipsisHolder *obj) { return true; }
  virtual void postVisitFVEllipsisHolder(FVEllipsisHolder *obj) {}

  virtual bool preVisitVariable(Variable_O *var) { return true; }
  virtual void postVisitVariable(Variable_O *var) {}
};

struct ValueAnnotation_O {
  bool usedInDataflow;

  ValueAnnotation_O() : usedInDataflow(false) {}
};

// this class represents abstract values for expressions and variables
//
// quarl 2006-06-16
//    Any annotation to be added to Value is added in the ValueFactory by
//    allocating a Struct<Value_<subtype>, Foo>; the getAnnotation0()
//    functions retrieve the corresponding object.
class Value : public TypeLike
//  INHERIT_SERIAL_BASE
#if USE_SERIAL_NUMBERS
, public SerialBase
#endif
{
  public:
  enum Tag {
    // VERY IMPORTANT: these tags are meant to be disjoint from those
    // of Type, so we start the counter after the last one in Type.
    V_ATOMIC = Type::T_LAST_TYPE_TAG + 1,// int const, class Foo, etc.
    V_POINTER,                  // int *
    V_REFERENCE,                // int &
    V_FUNCTION,                 // int ()(int, float)
    V_ARRAY,                    // int [3]
    V_POINTERTOMEMBER,          // int C::*
  };

  public:
  SourceLoc loc;                // at a location
  ReferenceValue *refToMe;      // the ref value that points to this
  Value *ptrToMe;               // the array or pointer (not ref) that points to this

  // set this once at the start of the program and don't use it again
  static bool allow_annotation;

  // tor
  Value(SourceLoc loc0);
  virtual ~Value() {}           // sm: silence gcc warning

  // each subclass has a type but the superclass doesn't; allow us to
  // pretend that it does
  virtual Type *t() const = 0;

  // roll-my-own RTTI
  virtual Tag getTag() const = 0;
  bool isCVAtomicValue()        const { return getTag() == V_ATOMIC; }
  bool isPointerValue()         const { return getTag() == V_POINTER; }
  bool isReferenceValue()       const { return getTag() == V_REFERENCE; }
  bool isFunctionValue()        const { return getTag() == V_FUNCTION; }
  bool isArrayValue()           const { return getTag() == V_ARRAY; }
  bool isPointerToMemberValue() const { return getTag() == V_POINTERTOMEMBER; }

  bool isPointerOrArrayValue()  const { Tag t = getTag(); return t == V_POINTER || t == V_ARRAY; }

  // checked downcasts
  DOWNCAST_FN(CVAtomicValue)
  DOWNCAST_FN(PointerValue)
  DOWNCAST_FN(ReferenceValue)
  DOWNCAST_FN(FunctionValue)
  DOWNCAST_FN(ArrayValue)
  DOWNCAST_FN(PointerToMemberValue)

  virtual int identifyByTag() const {return getTag();}
  virtual Type *asType() {xfailure("not a type");}
  virtual Type const *asTypeC() const {xfailure("not a type");}
  virtual Value *asValue() {return this;}
  virtual Value const *asValueC() const {return this;}

  virtual bool hasAtValue() {xfailure("should have been overridden");}
  virtual Value *getAtValue() {xfailure("should have been overridden");}
  SourceLoc getLoc() {return loc;}
  ReferenceValue *asLval();
  Value *asRval();

  // filter on all constructed types that appear in the type,
  // *including* parameter types; return true if any constructor
  // satisfies 'pred' (note that 1) recursive types always go through
  // CompoundType in the recursive cycle, and 2) anyCtorSatisfies()
  // does not dig into the fields of CompoundTypes; therefore we are
  // guaranteed to terminate.)  FIX: this is very redundant with
  // ValueVisitor.
  virtual bool anyCtorSatisfies(ValuePred &pred) const=0;

  // automatically wrap 'pred' in a StatelessValuePred;
  // trailing 'F' in name means "function", so as to avoid
  // overloading and overriding on the same name
  bool anyCtorSatisfiesF(ValuePredicateFunc *f) const;

  // invoke 'vis.visitType(this)', and then traverse subtrees
  virtual void traverse(ValueVisitor &vis) = 0;

  void setUsedInDataflow(bool usedInDataflow0);
  bool getUsedInDataflow() const;

public:
  // Get pointer to the annotation, which should live 'after' this (but such
  // computation depends on the size of *this, hence pure virtual).
  virtual void *getAnnotation0() = 0;

  // Get size of entire struct (if any)
  virtual size_t getWholeSize() = 0;
};

// I don't know why but gdb will not find if it is a method
void Value_gdb(Value *v);

// ****************

class BucketOValues;

// a leaf of a constructed value tree
class CVAtomicValue : public Value {
  typedef CVAtomicValue MyValue;
  typedef Struct<MyValue, ValueAnnotation_O> MyStruct;

  public:
  CVAtomicType *type;           // we are the abstract value of some type

  private:
  // for the int-as-void* feature, we need to treat any int as if it
  // were a void*, and therefore it needs to point at a void; this is
  // constructed lazily
  CVAtomicValue *atVoid;        // owner

  // union-find equivalence relation

  // quarl 2006-05-19
  //    We no longer use separate maps, instead use K->ufLink for union/find,
  //    and K->bucket for the bucket.

  CVAtomicValue *ufLinkForEquivRelOfDomOfMapToBucket; // union-find link

  BucketOValues *bucket;

public:
  friend class ValueFactory;
  friend class XmlValueWriter;
  friend class XmlValueReader;
  explicit CVAtomicValue()
    : Value((SourceLoc)0)
    , type(NULL)
    , atVoid(NULL)
    , ufLinkForEquivRelOfDomOfMapToBucket(NULL)
    , bucket(NULL)
  {}

protected:
  virtual void init(CVAtomicType *type0, SourceLoc loc0);

  virtual Tag getTag() const { return V_ATOMIC; }

public:
  virtual void *getAnnotation0() { return getAnnotation(); }
  ValueAnnotation_O *getAnnotation();

  virtual size_t getWholeSize() { return sizeof(MyStruct); }

  virtual Type *t() const { xassert(value2typeIsOn); xassert(type); return type; }

  // int-as-void* now includes ellipsis-as-void*; NOTE: you get
  // infinite loops if you attempt void-as-void*
  virtual bool hasAtValue();
  virtual Value *getAtValue();
  // NOTE: in general, you should not use this during the dataflow
  // analysis; just call getAtValue().  This is only here for the
  // serialization code to do an optimization.
  virtual bool hasAtVoid() {return atVoid != NULL;}

  // for instance-sensitive analyses, we need to get the
  // instance-specific type for a data-variable field
  Value *getInstanceSpecificValue(string const &memberName0, Value *shouldBeValue);

  virtual bool anyCtorSatisfies(ValuePred &pred) const;

  virtual void traverse(ValueVisitor &vis);

  // Unify two values and their buckets, and return the discarded bucket.
  BucketOValues *unifyBuckets(CVAtomicValue *tgt);

  // Get the bucket of the representative value.
  BucketOValues *getBucket() { return find()->bucket; }

  BucketOValues *getBucketCreate() { return find()->bucketCreate(); }

  void debugDumpLinkBucket();
  static void dumpBuckets();

private:
  // Get the representative CVAtomicValue.
  CVAtomicValue *find();

  // Force this CVAtomicValue to be the representative of its equivalence
  // class.
  void makeRepresentative();

  // Unify two CVAtomicValue (should only be called by unifyBuckets()).
  void unifyKey(CVAtomicValue *tgt);

  inline BucketOValues *bucketCreate();
};


// pointer to something
class PointerValue : public Value {
  typedef PointerValue MyValue;
  typedef Struct<MyValue, ValueAnnotation_O> MyStruct;

  public:
  PointerType *type;            // we are the abstract value of some type

  Value *atValue;               // owner

public:
  friend class ValueFactory;
  friend class XmlValueWriter;
  friend class XmlValueReader;
  explicit PointerValue()
    : Value((SourceLoc)0)
    , type(NULL)
    , atValue(NULL)
  {}

protected:
  virtual void init(PointerType *type0, SourceLoc loc0, Value *atValue0);

  virtual Tag getTag() const { return V_POINTER; }

public:
  virtual void *getAnnotation0() { return getAnnotation(); }
  ValueAnnotation_O *getAnnotation();

  virtual size_t getWholeSize() { return sizeof(MyStruct); }

  virtual Type *t() const { xassert(value2typeIsOn); xassert(type); return type; }

  virtual bool hasAtValue() {return true;}
  virtual Value *getAtValue() { return atValue; }

  virtual bool anyCtorSatisfies(ValuePred &pred) const;

  virtual void traverse(ValueVisitor &vis);
};


// reference to something
class ReferenceValue : public Value {
  typedef ReferenceValue MyValue;
  typedef Struct<MyValue, ValueAnnotation_O> MyStruct;

  public:
  ReferenceType *type;          // we are the abstract value of some type

  Value *atValue;               // owner

public:
  friend class Value;
  friend class ValueFactory;
  friend class XmlValueWriter;
  friend class XmlValueReader;
  explicit ReferenceValue()
    : Value((SourceLoc)0)
    , type(NULL)
    , atValue(NULL)
  {}

protected:
  virtual void init(ReferenceType *type0, SourceLoc loc0, Value *atValue0);

  virtual Tag getTag() const { return V_REFERENCE; }

public:
  virtual void *getAnnotation0() { return getAnnotation(); }
  ValueAnnotation_O *getAnnotation();

  virtual size_t getWholeSize() { return sizeof(MyStruct); }

  virtual Type *t() const { xassert(value2typeIsOn); xassert(type); return type; }

  virtual bool hasAtValue() {return true;}
  virtual Value *getAtValue() { return atValue; }

  virtual bool anyCtorSatisfies(ValuePred &pred) const;

  virtual void traverse(ValueVisitor &vis);
};


// provide a layer of indirection for the ellipsis variable
class FVEllipsisHolder {
  Variable_O *ellipsis;
  friend class FunctionValue;
  friend class XmlValueWriter;
  friend class XmlValueReader;

  public:
  explicit FVEllipsisHolder()
    : ellipsis(NULL)
  {}
  explicit FVEllipsisHolder(XmlReader&)
    : ellipsis(NULL)
  {}
};


class FunctionValue : public Value {
  typedef FunctionValue MyValue;
  typedef Struct<MyValue, ValueAnnotation_O> MyStruct;

public:
  FunctionType *type;           // we are the abstract value of some type

  Value *retValue;              // owner

  // When a function that attempts to return a CompoundType by value,
  // what really happens is that at the function call site, the
  // E_funCall, a temporary object of the return type is allocated on
  // the stack but not ctored.  A pointer to this object argument is
  // then passed down into the function to this parameter.  The
  // function then acts as a ctor function for initializing the
  // object: at some point it calls a real ctor with this object at
  // the "target".  This variable is not included in the list returned
  // by params().
  Variable_O *retVar;

  // list of function parameters; if (flags & FF_METHOD) then the
  // first parameter is '__receiver'
  SObjList<Variable_O> *params;

  private:
  // if acceptsVarargs(), then this holds information about the "..."
  // of a variable-argument function; it replaces the old ST_ELLIPSIS
  // parameter; NOTE: this is a pointer to a pointer as I need a layer
  // of indirection for dealing with FF_NO_PARAM_INFO unification
  // funnyness.  This is private so that clients are forced to go
  // through the get/set methods.
  FVEllipsisHolder *ellipsisHolder;
  friend void unifyEllipsisPtr0(DataFlowTy &dft, FunctionValue *a, FunctionValue *b);
  friend void unifyEllipsisPtr(DataFlowTy &dft, FunctionValue *a, FunctionValue *b);

  public:
  // allowable exceptions, if not NULL; not used for now
//    ExnSpec *exnSpec;             // (nullable owner)

public:
  friend class ValueFactory;
  friend class XmlValueWriter;
  friend class XmlValueReader;
  explicit FunctionValue()
    : Value((SourceLoc)0)
    , type(NULL)
    , retValue(NULL)
    , retVar(NULL)
    , params(new SObjList<Variable_O>)
    , ellipsisHolder(NULL)
  {}

protected:
  virtual void init(FunctionType *type0, SourceLoc loc0, Value *retValue0);

  virtual Tag getTag() const { return V_FUNCTION; }

public:
  virtual void *getAnnotation0() { return getAnnotation(); }
  ValueAnnotation_O *getAnnotation();

  virtual size_t getWholeSize() { return sizeof(MyStruct); }

  virtual Type *t() const { xassert(value2typeIsOn); xassert(type); return type; }

  virtual bool hasAtValue() { return true; }
    // xassert(0 && "3277a461-899e-4381-a487-c2d0dad40a97");
  virtual Value *getAtValue() {
    // We treat functions also as function pointers to themselves.  This works
    // around a bug in elsa.  See Test/funccast_to_nonfunc1.cc.
    return this;
    // xassert(0);
  }

  // continuation of constructor, essentially
  int maxNthParamIndex();
  Value *nthParamAbstrValue(int n);

  Variable_O *getRetVar();
  void registerRetVar(Variable *retVar0);

  bool hasEllipsis() const;
  Variable_O *getEllipsis();
  void setEllipsis(Variable_O *ellipsis0);
  Variable_O *ensureEllipsis();

  // is this function type from a K and R declaration (including
  // definitions)
  bool isKandR();

  virtual bool anyCtorSatisfies(ValuePred &pred) const;

  virtual void traverse(ValueVisitor &vis);
};


// array
class ArrayValue : public Value {
  typedef ArrayValue MyValue;
  typedef Struct<MyValue, ValueAnnotation_O> MyStruct;

public:
  ArrayType *type;              // we are the abstract value of some type

  Value *atValue;

  // This exists to make an ArrayValue more parallel to a
  // CompoundValue.  Such parallelism is needed to make the
  // implementation of instance sensitivive compound initialziers
  // easier.  There are alternatives, but they are more complex.  I
  // will make this field lazily and therefore it should only ever
  // exist if it is needed.
  Variable_O *atVar;

public:
  friend class ValueFactory;
  friend class XmlValueWriter;
  friend class XmlValueReader;
  explicit ArrayValue()
    : Value((SourceLoc)0)
    , type(NULL)
    , atValue(NULL)
    , atVar(NULL)               // made lazily
  {}

  virtual void init(ArrayType *type0, SourceLoc loc0, Value *atValue0);

  virtual Tag getTag() const { return V_ARRAY; }

public:
  virtual void *getAnnotation0() { return getAnnotation(); }
  ValueAnnotation_O *getAnnotation();

  virtual size_t getWholeSize() { return sizeof(MyStruct); }

  virtual Type *t() const { xassert(value2typeIsOn); xassert(type); return type; }

  virtual bool hasAtValue() {return true;}
  virtual Value *getAtValue() { return atValue; }

  virtual Variable_O *getAtVar();

  virtual bool anyCtorSatisfies(ValuePred &pred) const;

  virtual void traverse(ValueVisitor &vis);
};


// pointer to member
class PointerToMemberValue : public Value {
  typedef PointerToMemberValue MyValue;
  typedef Struct<MyValue, ValueAnnotation_O> MyStruct;

public:
  PointerToMemberType *type;    // we are the abstract value of some type

  Value *atValue;

  // NOTE: the named atomic type (NAT) will be a compound type or a
  // template parameter.  These have typedef vars which will have
  // their own values.

public:
  friend class ValueFactory;
  friend class XmlValueWriter;
  friend class XmlValueReader;
  explicit PointerToMemberValue()
    : Value((SourceLoc)0)
    , type(NULL)
    , atValue(NULL)
  {}

protected:
  virtual void init(PointerToMemberType *type0, SourceLoc loc0, Value *atValue0);

  virtual Tag getTag() const { return V_POINTERTOMEMBER; }

public:
  virtual void *getAnnotation0() { return getAnnotation(); }
  ValueAnnotation_O *getAnnotation();

  virtual size_t getWholeSize() { return sizeof(MyStruct); }

  virtual Type *t() const { xassert(value2typeIsOn); xassert(type); return type; }

  virtual bool hasAtValue() {return true;}
  virtual Value *getAtValue() { return atValue; }

  virtual bool anyCtorSatisfies(ValuePred &pred) const;

  virtual void traverse(ValueVisitor &vis);
};

// **** ParamIter

// sm: this replaces SObjListIterNB<Variable_O> so the iterators can
// continue to pretend that ST_ELLIPSIS is the last parameter of
// variable-argument functions
class ParamIter {
private:
  // underlying list iterator
  SObjListIterNC<Variable_O> iter;

  FunctionValue *fv;

private:
  explicit ParamIter(ParamIter&); // disallowed for now

public:
  explicit ParamIter(FunctionValue *fv0);

  bool isDone() const;
  // I think this should never be used; use
  // advanceButNotPastEllipsis() below
//   void adv();
  Variable_O *data() const;

  // special: advance, but if we're on the ellipsis parameter then
  // just stay there
  void advanceButNotPastEllipsis();
};

// for iterating over the rest of the parameters on the
// FunctionValue::params of a FF_NO_PARAM_INFO Variable
class ParamIter_NO_PARAM_INFO {
private:
  FunctionValue *fv;
  Variable_O *phantomVar;

private:
  explicit ParamIter_NO_PARAM_INFO(ParamIter_NO_PARAM_INFO&); // disallowed for now

public:
  explicit ParamIter_NO_PARAM_INFO(FunctionValue *fv0);

  // it is never done; we just keep manufacturing parameters
//   bool isDone() const;
  void adv();
  // NOTE: I pass in the type of the matching parameter so that we can
  // attempt to mirror it in the phantom parameter; we used to just
  // made all phantom params 'int' but that doesn't work for matching
  // parameters that are a CompoundType
  Variable_O *data(Type *matchingParamType);
};


// ****************

// A map from strings to types with special merge semantics when it
// encounters another.  The two use-cases are detailed below.
//
// instance-sensitivity: Used to represent the instances of a CVAtomicType
// by mapping the field names to the types.
//
// void-polymorphism: Used in a different way for the various
// interpretations of a void (as in, the thing pointed to by a void*)
// by mapping the mangled name of type of the partner in the edge
// insertion to a type for void used in that circumstance.

class BucketOValues {
  StringSObjDict<Value> nameToValue;
  // dsw: I took the trouble to turn this off because it would
  // increase the size of the object and we are going to have a lot of
  // these; UPDATE: on the other hand, it is just one boolean compared
  // to a whole hashtable, so I'm leaving it on for now.
//  #if !defined(NDEBUG)
  bool enabled;
//  #endif

  public:
  explicit BucketOValues();

  private:
  explicit BucketOValues(BucketOValues const &); // prohibit
  BucketOValues const & operator =(BucketOValues const &); // prohibit
  friend class XmlValueWriter;
  friend class XmlValueReader;

  public:
  // make this bucket no longer usable
  void disable();
  bool isEnabled() {return enabled;}
  // get the value for a given key
  Value *get(string const &key);
  // DO NOT USE THIS METHOD; It exists just to break a cyclic
  // dependency while copying out data while de-commissioning a bucket
  Value *getEvenThoughDisabled(string const &key);
  // put the value for a key
  void put(string const &key, Value *value);
  // for all the fields that the other has that we do not, copy them
  // to us
  void getOthersExtraFields(BucketOValues &other);
  // for debugging
  void dump(int indentation);

  class Iter {
    StringSObjDict<Value>::Iter iter0;

    public:
    explicit Iter(BucketOValues &bov) : iter0(bov.nameToValue) {
      xassert(bov.enabled);
    }
    private:
    explicit Iter(Iter &other) : iter0(other.iter0) {}
    friend class BucketOValues;

    // API
    public:
    bool isDone()       {return iter0.isDone();}
    void adv()          {iter0.next();         }
    string const &key() {return iter0.key();   }
    Value *&value()     {return iter0.value(); }
  };

  // iterate over fields in common between this and other's fields
  class PairIter {
    BucketOValues const &thisBucket;
    StringSObjDict<Value>::Iter otherIter;
    BucketOValues const &otherBucket;

    public:
    explicit PairIter(BucketOValues &thisBucket0, BucketOValues &other);

    private:
    explicit PairIter();        // prohibit
    PairIter const &operator = (PairIter const &); // prohibit
    void stopAtNextMatch();

    public:
    bool isDone();
    void adv();
    string data();              // FIX: should this be a string ref?
  };

  // sm: cppstd 11.8p1 is quite clear that PairIter should not be able
  // to access private members of BucketOValues without this 'friend'
  // declaration, but both gcc-3 and icc allow it to be missing; I am
  // adding this to pacify gcc-2, which seems to implement the right
  // rules
  friend class PairIter;
};

// TODO: this is only down here since BucketOValues wasn't defined yet;
// class BucketOValues should be moved above class Value.
inline BucketOValues *CVAtomicValue::bucketCreate() {
  if (bucket == NULL) bucket = new BucketOValues;
  return bucket;
}


// ****************

class ValueFactory {
public:
  friend class Value;
  typedef IterableBlockAllocator Allocator;
  typedef Allocator::Iter Iter;

protected:
  Allocator allocator;

public:
  explicit ValueFactory() {}
  virtual ~ValueFactory() {}

  Allocator &getAlloc() { return allocator; }
  size_t getNumValues() const { return allocator.getAllocCount(); }

  // those outside who need to call them go through the typefactory
  // functions below

public:
  // quarl 2006-06-16
  //    These just allocate a new object of the named type.  Children may want
  //    to use Struct (from alloc.h) to add their own data.
  virtual CVAtomicValue        *alloc_CVAtomicValue();
  virtual PointerValue         *alloc_PointerValue();
  virtual ReferenceValue       *alloc_ReferenceValue();
  virtual FunctionValue        *alloc_FunctionValue();
  virtual ArrayValue           *alloc_ArrayValue();
  virtual PointerToMemberValue *alloc_PointerToMemberValue();

private:
  virtual CVAtomicValue        *buildCVAtomicValue(CVAtomicType *src,   SourceLoc loc);
  virtual PointerValue         *buildPointerValue(PointerType *src,     SourceLoc loc);
  virtual ReferenceValue       *buildReferenceValue(ReferenceType *src, SourceLoc loc);
  virtual FunctionValue        *buildFunctionValue(FunctionType *src,   SourceLoc loc);
  virtual ArrayValue           *buildArrayValue(ArrayType *src,         SourceLoc loc);
  virtual PointerToMemberValue *buildPointerToMemberValue
    (PointerToMemberType *src, SourceLoc loc);

public:
  virtual Value *buildValue(Type *src, SourceLoc loc);

  virtual CVAtomicValue *makeAtVoid(CVAtomicValue const *v) const;

  virtual void finishJustClonedMember(CVAtomicValue *v, string memberName, Value *memberType) {}
};

#define FOREACH_VALUE(iter) for (ValueFactory::Iter iter(vFac->getAlloc()); !iter.isDone(); iter.adv())

// does this value have an autounion when instance_sensitive is on?
bool isAutoUnionValue(Value *value);

#endif // VALUE_H
