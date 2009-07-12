// see License.txt for copyright and terms of use

// Extensions/modifications made to the oink typesystem.  Note that
// oink_type.h is a strange header file that inserts a class into the
// *middle* of the Elsa Type hierarchy; this simple preprocessor trick
// prevents the need for multiple inheritance.

    // NOTE NOTE NOTE: Oink files should NOT include this file
    // directly; it is included into cc_type.h by a special hack in
    // the build process.  Oink files should include cc_type.h

#ifndef CC_TYPE_INCLUDE_CLASS_FILE_FLAG
#  error Do not include this file directly; include cc_type.h instead.  See the comment a the top of oink_type.h.
#endif

// The essential idea is to let a client analysis (Oink in this case)
// have a representation of C++ types which it can manipulate with
// arbitrary semantics, and leverage the information represented in
// cc_type, while not polluting cc_type with annotations or burdening
// it with analysis semantics.

#ifndef OINK_TYPE_H
#define OINK_TYPE_H

// from oink_type_children.h
class TypeFactory_O;

// from qual_literal.h; This is just an opaque thing here
class QLiterals;

// forwards in this file
class Type;
class Value;

// ****************

// A Type or a Value
class TypeLike {
  public:
  virtual ~TypeLike() {}
  // return the tag of the Type or Value; recall that the Tag/Value
  // space is guaranteed to be disjoint
  virtual int identifyByTag() const = 0;
  // is this a value?  Implemented in terms of the above function.
  int isActuallyAValue() const;
  virtual Type *asType() = 0;
  virtual Type const *asTypeC() const = 0;
  virtual Value *asValue() = 0;
  virtual Value const *asValueC() const = 0;
};

// default class Type copied and modified from elsa/cc_type.h
class Type : public TypeLike, public BaseType {
  public:                       // funcs
  // do not leak the name "BaseType"
  Type const *asRvalC() const
  { return static_cast<Type const *>(BaseType::asRvalC()); }
  Type *asRval()
  { return static_cast<Type*>(BaseType::asRval()); }

  // filter on all type constructors (i.e. non-atomic types); return
  // true if any constructor satisfies 'pred'
  virtual bool anyCtorSatisfies(TypePred &pred) const=0;

  // end default class Type, start extensions ****************

  virtual int identifyByTag() const {return getTag();}
  virtual Type *asType() {return this;}
  virtual Type const *asTypeC() const {return this;}
  virtual Value *asValue() {xfailure("not a value");}
  virtual Value const *asValueC() const {xfailure("not a value");}

  public:
  QLiterals *ql;                // any literals we were qualified with; this is just opaque

protected:                      // funcs
  // the constructor is protected to enforce using the factory
  Type()
    : ql(NULL)
  {}
  friend class TypeFactory_O;

public:
  virtual ~Type() {}
};

#endif // OINK_TYPE_H
